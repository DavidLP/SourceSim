#include <vector>
#include <cmath>

#include "Digitizer.hh"
#include "DigitizerMessenger.hh"
#include "PixelDigi.hh"
#include "DetHit.hh"

#include "G4RunManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"
#include "G4PhysicalConstants.hh"

#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PixelROWorld.hh"

Digitizer::Digitizer(G4String name) :
		G4VDigitizerModule(name),
		fCalcChargeCloud(true), // distribute charge on neigbouring pixels due to charge cloud
		fReadOutDirection(true), // True: sensor first then readout in beam direction
		fThreshold(2000),  // std. IBL detector
		fNoise(130),  // std. IBL detector
		fTemperatur(330),  // 56.8 C (uncooled 50-60 is common)
		fBias(80.),  // bias of the sensor in volt
		fVdep(45.),  // depletion voltage of sensor in volt
		fSigma0(0. * um),  // initial charge cloud gaussian profile sigma, if 0 calculate from approximation
		fSigmaCC(1.), // correction factor for charge cloud sigma(z) to take into account also repulsion
		fNtype(true),  // ntype detector otherwise ptype
		fRepulsion(false),  // calculates charge cloud sigma with repulsion, needs fSigma0 != 0
		fTriggerHits(false), // trigger: create digits only if trigger volume is hit //FIXME
		fPixelDigitsCollection(0), fTriggerHCID(-1)
{
	G4String colName = "PixelDigitsCollection";
	collectionName.push_back(colName);

	digiMessenger = new DigitizerMessenger(this);

	// Get pixel detector info needed for digitization
	DetectorConstruction const* detector =
			dynamic_cast<DetectorConstruction const*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	PixelROWorld* pWorld =
			dynamic_cast<PixelROWorld*>(detector->GetParallelWorld(0));
	fSensorThickness = pWorld->GetSensorThickness();
	fPixelPitchX = pWorld->GetPixelPitchX();
	fPixelPitchY = pWorld->GetPixelPitchY();
	fNcolumns = pWorld->GetNcolumns();
	fNrows = pWorld->GetNrows();

	// Mobility value not important, since it cancelles out here for cc width calc
	fMu = 1150. * 1e4 * 1e4 / 1e6; // um2 / us V

	fEHPerChargeElectrons = getEHenergy(fTemperatur); // getEHenergyElectrons(fTemperatur);
	fEHPerChargePhotons = getEHenergy(fTemperatur); // getEHenergyPhotons(fTemperatur);

	PrintSettings();

//	std::cout<<"Sigma diff "<<CalcSigmaDiffusion(200*um, 80., fTemperatur, 16000.);
//	fRepulsion=true;
//	fSigma0 = 0.0873564802323;
//	std::cout<<"Sigma diff rep "<<CalcSigmaDiffusion(200*um, 80., fTemperatur, 16000.);

}

Digitizer::~Digitizer()
{
	delete digiMessenger;
}

void Digitizer::Digitize()
{
	G4DigiManager* digiMan = G4DigiManager::GetDMpointer();

	// Create a temporary actual event digits collection, to be able to remove hits below threshold, G4TDigiCollection does not support deleting entries ?!
	PixelDigitsCollection* actualPixelDigitsCollection =
			new PixelDigitsCollection("PixelDigitizer",
					"PixelDigitsCollection");
	// Create the result actual event digits collection
	fPixelDigitsCollection = new PixelDigitsCollection("PixelDigitizer",
			"PixelDigitsCollection");

	// Trigger machanism
	fTriggerHCID = G4SDManager::GetSDMpointer()->GetCollectionID(
			"Trigger/TriggerMechanism");
	if (fTriggerHits && fTriggerHCID != -1)
	{ // check if there is a trigger volume and if the detector hits are triggered
		const G4Event* event =
				G4EventManager::GetEventManager()->GetConstCurrentEvent();
		G4THitsMap<G4double>* triggerHits =
				static_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(
						fTriggerHCID));
		if (triggerHits->GetSize() == 0)
		{  // abort digitization if the trigger is not hit in actual event
			StoreDigiCollection(fPixelDigitsCollection);
			return;
		}
	}

	// Get actual event hits collection
	G4int pixelDetectorHCID = G4SDManager::GetSDMpointer()->GetCollectionID(
			"PixelDetektor/PixelHitCollection");
	DetHitsMap* hitsCollection =
			const_cast<DetHitsMap*>(static_cast<const DetHitsMap*>(digiMan->GetHitsCollection(
					pixelDetectorHCID))); // a beautiful line to show what can go wrong in C++ frameworks when too less time is spend on the API...
	if (!hitsCollection)
		G4Exception("Digitizer::Digitize()",
				"Cannot access pixel hits collection", FatalException, "");

//	if (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() % 1 == 0)
//		std::cout<<"\nEVENT: "<<G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID()<<G4endl;
//	PrintSettings();

	// This is a loop with an inner loop; speed wise not the best solution, but the geant4 examples are even worse and loop all hits + digits (1 pixel = 1 digit...), here the least amount of hits / digits were created and are looped
	for (std::map<G4int, DetHit*>::const_iterator it =
			hitsCollection->GetMap()->begin();
			it != hitsCollection->GetMap()->end(); ++it)
	{
		if (it->second->GetVolumeIdX() == -1) // speed up, do not loop remaining and all empty DetHits
			break;
		AddHitToDigits(it, actualPixelDigitsCollection);
	}

	// Apply noise and threshold to all actual pixel digis
	for (G4int iDigi = 0; iDigi < actualPixelDigitsCollection->entries();
			++iDigi)
	{
//		G4cout << "\nDIGIT: " << (*actualPixelDigitsCollection)[iDigi]->GetColumn() << "/" << (*actualPixelDigitsCollection)[iDigi]->GetRow() << "/" << (*actualPixelDigitsCollection)[iDigi]->GetCharge() << G4endl;

		(*actualPixelDigitsCollection)[iDigi]->SetCharge(G4RandGauss::shoot((*actualPixelDigitsCollection)[iDigi]->GetCharge(), fNoise)); // add gaussian noise to the charge
		if ((*actualPixelDigitsCollection)[iDigi]->GetCharge() >= fThreshold)
		{
			fPixelDigitsCollection->insert(new PixelDigi(*(*actualPixelDigitsCollection)[iDigi]));
//			G4cout << "\nADD DIGIT: " << (*actualPixelDigitsCollection)[iDigi]->GetColumn() << "/" << (*actualPixelDigitsCollection)[iDigi]->GetRow() << "/" << (*actualPixelDigitsCollection)[iDigi]->GetCharge() << G4endl;
		}
	}

	StoreDigiCollection(fPixelDigitsCollection);
}

void Digitizer::AddHitToDigits(std::map<G4int, DetHit*>::const_iterator iHit,
		PixelDigitsCollection* digits)
{
	// Set the total charge per digi pixel by summing all hits within the different pixel volumes (= 2D charge histograming)
	// If activated calculate charge sharing due to diffusion
	G4int column = iHit->second->GetVolumeIdX();
	G4int row = iHit->second->GetVolumeIdY();
	G4double charge = iHit->second->GetEdep();

	if (charge == 0)
	{
		return;
	}

	if (iHit->second->GetParticle() == "gamma")
	{
		charge /= (fEHPerChargePhotons * eV); // Charge in electrons
	}
	else
	{
		charge /= (fEHPerChargeElectrons * eV); // Charge in electrons
	}

	G4ThreeVector position = iHit->second->GetPosition(); // are negative in z or not?

//	G4cout << "\nADD HIT: " << column << "/" << row << "/" << iHit->second->GetParticle() << G4endl;
//	G4cout << "    Position: " << G4BestUnit(position, "Length") << G4endl;
//	G4cout << "    CHARGE: " << charge << " e" << G4endl;

	if (fCalcChargeCloud)
	{  // distribute the deposited charge into neighboring pixels
	   // temporary variables
		double z = 0;  // z position in sensor [0..thickness]
		double fraction = 0; // fraction of the total charge for the actual pixel
		double minFraction = 1e-5; //  minimum charge fraction to add charge to the actual pixel
		double totalFraction = 0;  // total fraction of charge distributed (=1)

		if (fReadOutDirection) // calculate drift/read out electronics direction
			z = fSensorThickness / 2. - position[2];  // RO after sensor in beam
		else
			z = fSensorThickness / 2. + position[2]; // RO before sensor in beam

		// Fix instabilities in GEANT4; I guess
		if (z < 0)
			z = 0.;
		if (z > fSensorThickness)
			z = fSensorThickness;

		//G4cout << " z before " << G4BestUnit(z, "Length") << G4endl;

		// Increase z to simulate a charge cloud with non zero spread at start,
//		if (iHit->second->GetParticle() != "gamma") // Used for charged particles where no reasonable theory exists, I guess
//		{
////			G4cout << " fSigma0 " << G4BestUnit(fSigma0, "Length") << G4endl;
//			if (fSigma0 > 0)
//				z = CalcZfromSigma(z, fSigma0);
//		}
		// Gamma charge cloud size is simulated by the secondary electrons in GEANT4!
		// MC correct like in http://dx.doi.org/10.1016/j.nima.2010.11.173
		// But primary gamma interaction might be without photo electron to save time
		// Then use this approximation
//		else
//		{
////			G4cout << "GAMMA fSigma0 " << G4BestUnit(fSigma0, "Length") << G4endl;
//			z = CalcZfromSigma(z, getSigmaPhotons(iHit->second->GetEdep()));
//		}

		//G4cout << " z after " << G4BestUnit(z, "Length") << G4endl;
//		if (z == 0)
//		{  // Special case. charge does not travel at all, does not exist anymore due to repulsion
//			AddChargeToDigits(column, row, charge, digits);
//			return;
//		}

//		std::cout<<"fNcolumns "<<fNcolumns<<std::endl;
//		std::cout<<"fNrows "<<fNrows<<std::endl;
//		std::cout<<"column "<<column<<std::endl;
//		std::cout<<"row "<<row<<std::endl;

		// run time optimized loops using an abort condition utilizing that the charge sharing always decreases for increased distance to seed pixel and the fact that the total charge fraction sum is 1
		for (int iColumn = 0; column + iColumn < fNcolumns; ++iColumn)
		{  // calc charge in pixels in column direction
			if (totalFraction == 1.
					|| CalcChargeFraction(position[0], position[1], z, charge,
							fPixelPitchX, fPixelPitchY, fBias, iColumn, 0,
							fTemperatur) < minFraction) // omit row loop if charge fraction is already too low for seed row (=0)
				break;
			for (int iRow = 0; iRow < fNrows; ++iRow)
			{  // calc charge in pixels in row direction
				fraction = CalcChargeFraction(position[0], position[1], z, charge,
						fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow,
						fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction) //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
//				G4cout << "Col/Row/Fraction: " << column + iColumn <<"\t"<<row + iRow<<"\t"<<fraction <<
//						"\t"<<G4BestUnit(charge * fEHPerChargeElectrons * eV, "Energy")<< "\t"<<G4BestUnit(position[0], "Length")<< "\t"<<G4BestUnit(position[1], "Length")<<G4endl;
				AddChargeToDigits(column + iColumn, row + iRow,
						charge * fraction, digits);
				if (totalFraction == 1.) // abort if all charge already distributed
					break;
			}
			for (int iRow = -1; row + iRow >= 0; --iRow)
			{  // calc charge in pixels in -row direction
				fraction = CalcChargeFraction(position[0], position[1], z, charge,
						fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow,
						fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction) //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
//				G4cout << "Col/Row/Fraction: " << column + iColumn <<"\t"<<row + iRow<<"\t"<<fraction << G4endl;
				AddChargeToDigits(column + iColumn, row + iRow,
						charge * fraction, digits);
				if (totalFraction == 1.) // abort if all charge already distributed
					break;
			}
		}
		for (int iColumn = -1; column + iColumn >= 0; --iColumn)
		{  // calc charge in pixels in -column direction
			if (totalFraction == 1.
					|| CalcChargeFraction(position[0], position[1], z, charge,
							fPixelPitchX, fPixelPitchY, fBias, iColumn, 0,
							fTemperatur) < minFraction) // omit row loop if charge fraction is already too low for seed row (=0)
				break;
			for (int iRow = 0; iRow < fNrows; ++iRow)
			{  // calc charge in pixels in row direction
				fraction = CalcChargeFraction(position[0], position[1], z, charge,
						fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow,
						fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction) //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
//				G4cout << "Col/Row/Fraction: " << column + iColumn <<"\t"<<row + iRow<<"\t"<<fraction << G4endl;
				AddChargeToDigits(column + iColumn, row + iRow,
						charge * fraction, digits);
				if (totalFraction == 1.) // abort if all charge already distributed
					break;
			}
			for (int iRow = -1; row + iRow >= 0; --iRow)
			{  // calc charge in pixels in -row direction
				fraction = CalcChargeFraction(position[0], position[1], z, charge,
						fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow,
						fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction) //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
//				G4cout << "Col/Row/Fraction: " << column + iColumn <<"\t"<<row + iRow<<"\t"<<fraction << G4endl;
				AddChargeToDigits(column + iColumn, row + iRow,
						charge * fraction, digits);
				if (totalFraction == 1.) // abort if all charge already distributed
					break;
			}
		}
	}
	else
	{
		AddChargeToDigits(column, row, charge, digits);
	}
}

void Digitizer::AddChargeToDigits(const int& column, const int& row,
		const double& charge, PixelDigitsCollection* digits)
{
// Runtime ugly function, a two key hash map would be nice...
	bool digitExists = false;
	for (G4int iDigi = 0; iDigi < digits->entries(); ++iDigi)
	{  // go through all already created digis, if it exists add charge
		if (column == (*digits)[iDigi]->GetColumn()
				&& row == (*digits)[iDigi]->GetRow())
		{
			(*digits)[iDigi]->Add(charge);
			digitExists = true;
			break;
		}
	}

	if (!digitExists)
	{  // pixel digi does not exist, thus create new digit with given charge
		PixelDigi* digi = new PixelDigi(column, row, charge);
		digits->insert(digi);
	}
}

double Digitizer::CalcChargeFraction(const double& x, const double& y,
		const double& z, const double& charge, const double& x_pitch, const double& y_pitch,
		const double& voltage, const int& x_pixel_offset,
		const int& y_pixel_offset, const double& temperature)
{
	/* Calculates the fraction of charge [0, 1] within one rectangular pixel volume when diffusion is considered. The calculation is done within the local pixel coordinate system, with the origin [x_pitch / 2, y_pitch / 2, 0]
	 x, y : position where the charge fraction has to be measured
	 x_pitch, y_pitch : pixel dimensions in x and y
	 x_pixel_offset, y_pixel_offset : pixel index relative to the center pixel where the charge fraction has to be calculated
	 voltage : the applied voltage
	 temperature : the temperature in Kelvin
	 */
	double sigma = CalcSigmaDiffusion(z, voltage, temperature, charge);

	if (!fRepulsion)
		sigma = std::sqrt(fSigma0 * fSigma0 + sigma * sigma);  // add sigma0 at t=0

	sigma *= fSigmaCC; // apply correction factor to take repulsion into account, when needed to describe data

//	std::cout<<"\tsigma "<<G4BestUnit(sigma, "Length")<<"\t"<<charge<<std::endl;

	// Tread not defined calculation input
	if (sigma == 0){
		if (x_pixel_offset == 0 && y_pixel_offset == 0)
			return 1.;
		return 0.;
	}

	return CalcBivarianteNormalCDFWithLimits(
			x_pitch * (x_pixel_offset - 1. / 2.),
			x_pitch * (x_pixel_offset + 1. / 2.),
			y_pitch * (y_pixel_offset - 1. / 2.),
			y_pitch * (y_pixel_offset + 1. / 2.), x, y, sigma);
}

double Digitizer::CalcSigmaDiffusion(const double& length,
		const double& voltage, const double& temperature, const double& charge)
{
	/* Calculates the sigma of the diffusion by using the drift time estimate from the
	 * analytical E-Field solution for a planar detector.
	 * When repulsion is activated the sigma is calculate taking repulsion into account.
	 * Ansatz from: Nuclear Instruments and Methods in Physics Research A 606 (2009) 508–516
	 * with assumption of gaussian cc.
	 length : the drift length
	 voltage : the applied voltage
	 temperature : the temperature in Kelvin
	 */
	const double kb_K_e = 8.6173e-5; // Boltzmann Constant * Kelvin / elemetary charge
//	std::cout<<"fSensorThickness\t"<<fSensorThickness<<"\n";
//	std::cout<<"length\t"<<length<<"\n";
//	std::cout<<"temperature\t"<<temperature<<"\n";
//	std::cout<<"voltage\t"<<voltage<<"\n";
//	std::cout<<"fVdep\t"<<fVdep<<"\n";
//
//	G4cout<<"OLD "<<length * std::sqrt(2. * temperature / voltage * kb_K_e)<<G4endl;

	double drift_time = CalcDriftTime(length, voltage);

	double dt = 1e-3;  // 0.1 ps
	double t = 0.;  // time
	double s_t_2 = fSigma0;  // sigma squared

	double epsilon_s_e = 6.24e2;  // Permittivity of silicon [F/um = C/(V*um) = e/(V*um)]]

//	std::cout<<"drift_time "<<drift_time<<std::endl;
//	std::cout<<"temperature "<<temperature<<std::endl;
//	std::cout<<"epsilon_s_e "<<epsilon_s_e<<std::endl;
//	std::cout<<"fMu "<<fMu<<std::endl;
//	std::cout<<"charge "<<charge<<std::endl;
//	std::cout<<"fSigma0 "<<fSigma0<<std::endl;

	double D = getD();
//	std::cout<<"D "<<D<<std::endl;
	if (fRepulsion) {
		// Time evolution can only be calculated for sigma(t0) != 0.
		// Thus set a sigma0 if needed
		if (s_t_2 == 0.){
			s_t_2 = getSigmaPhotons(charge * (fEHPerChargePhotons * eV));
//			std::cout<<"s_t_2 "<<G4BestUnit(s_t_2, "Length")<<std::endl;
//			std::cout<<"charge "<<charge<<std::endl;
//			std::cout<<"energy "<<G4BestUnit(charge * (fEHPerChargePhotons * eV), "Energy")<<std::endl;
//			std::cout<<"------------------"<<std::endl;
//			G4Exception("Digitizer::CalcSigmaDiffusion",
//			                        "Error", FatalException, "Charge cloud evolution with time including repulsion can only be calculated for finite charge cloud width at start!");
		}

		// Stability criterion
		// sigma0 = 0.1 - 0.5 um is const.
		// but sigma 0 < 0.1 um can lead to
		// numerical instabilities
		if (s_t_2 < 0.1*um)
			s_t_2 = 0.1*um;
//		std::cout<<"s_t_2 "<<G4BestUnit(s_t_2, "Length");
		// Simple numerical integration
		while (t < drift_time){
			double D_add = fMu * charge / (24. * pow(pi, 3. / 2.) * epsilon_s_e * s_t_2);
			s_t_2 += 2. * (D + D_add) * dt * 1e-3;
			t += dt;
		}

		return std::sqrt(s_t_2) * um;
	}

	//FIXME: calculate units right, this is ugly
	double sigma = std::sqrt(2. * fMu * um * um * 1e-3 * kb_K_e * temperature * drift_time);

//	std::cout<<"sigma "<<G4BestUnit(sigma, "Length")<<std::endl;

//	G4cout<<"NEW sigma "<<G4BestUnit(tmp_s, "Length")<<G4endl;
//	G4cout<<"ptype sigma "<<G4BestUnit(fSensorThickness * std::sqrt(temperature * kb_K_e / fVdep) *
//			   std::sqrt(-std::log(1. - 2./fSensorThickness * fVdep / (voltage + fVdep) * length)), "Length")<<G4endl;
//	G4cout<<"ntpye sigma "<<G4BestUnit(fSensorThickness * std::sqrt(temperature * kb_K_e / fVdep) *
//			   std::sqrt(std::log(1. + 2./fSensorThickness * fVdep / (voltage - fVdep) * length)), "Length")<<G4endl;
//	G4cout<<"NEW drift time "<<G4BestUnit(tmp_t, "Time")<<G4endl;

//	if (!fNtype){
//		double sig_old = fSensorThickness * std::sqrt(temperature * kb_K_e / fVdep) * std::sqrt(-std::log(1. - 2./fSensorThickness * fVdep / (voltage + fVdep) * length));
//			if(sigma != sig_old){
//				std::cout<<"sigma "<<sigma<<std::endl;
//				std::cout<<"sig_old "<<sig_old<<std::endl;
//				assert(false==true);
//			}
//		}
//	else{
//		double sig_old = fSensorThickness * std::sqrt(temperature * kb_K_e / fVdep) * std::sqrt(std::log(1. + 2./fSensorThickness * fVdep / (voltage - fVdep) * length));
//		if(sigma != sig_old){
//						std::cout<<"sigma "<<sigma<<std::endl;
//						std::cout<<"sig_old "<<sig_old<<std::endl;
//						assert(false==true);
//					};
//	}
	return sigma;
}

double Digitizer::CalcDriftTime(const double& length, const double& voltage)
{
	/* Calculates the drift time from the
	 * analytical E-Field solution for a planar detector.
	 length : the drift length
	 voltage : the applied voltage
	 temperature : the temperature in Kelvin
	 */
	if (!fNtype){
		return -std::pow(fSensorThickness/um, 2) / (2. * fMu * fVdep)
				* std::log(1. - 2. / (fSensorThickness/um) * fVdep / (voltage + fVdep) * length/um) * 1e3;
	}

	return std::pow(fSensorThickness/um, 2) / (2. * fMu * fVdep)
					* std::log(1. + 2. / (fSensorThickness/um) * fVdep / (voltage - fVdep) * length/um) * 1e3;
}

double Digitizer::CalcBivarianteNormalCDFWithLimits(const double& a1,
		const double& a2, const double& b1, const double& b2, const double& mu1,
		const double& mu2, const double& sigma)
{
// Calculates the integral of the bivariante normal distribution between x = [a1, a2], y = [b1, b2]. The normal distribution has two mu: mu1, mu2 but only one common sigma.
	return 1. / 4.
			* (erf((a2 - mu1) / std::sqrt(2 * sigma * sigma))
					- erf((a1 - mu1) / std::sqrt(2 * sigma * sigma)))
			* (erf((b2 - mu2) / std::sqrt(2 * sigma * sigma))
					- erf((b1 - mu2) / std::sqrt(2 * sigma * sigma)));
}

//double Digitizer::CalcZfromSigma(const double& z, const double& sigma)
//{
//	// To simulate the initial charge cloud the z-position of the hit is scales up to
//	// give a gaussian with sigma0 at the old z-position.
//	// The new z cannot be calculated analytically
//	// sigma_new^2 * ln(z / z_new) - sigma^2 = 0 has to be solved numerically
//	// This is the easiest and stupiest way of doing it ...
//	for (double zNew = z; zNew < z + 2 * fSensorThickness; zNew += 0.5 * um)
//	{  // increase z_new until close solution is found
//		double actual_sigma = CalcSigmaDiffusion(zNew, fBias, fTemperatur);
//		double minimizeme = 2 * actual_sigma * actual_sigma * std::log(zNew / z)
//				- sigma * sigma;  // function to minimize
//		if (minimizeme > 0){ // function has 0 crossing, take this as the minimum
//			G4cout<<"ADDED Z\t"<<zNew - z<<"\n";
//			return zNew;
//		}
//
////		if (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() == 1046){
////			G4cout << "    sigma " << G4BestUnit(sigma, "Length") << G4endl;
////			G4cout << "    z " << G4BestUnit(z, "Length") << G4endl;
////			G4cout << "    zNew " << G4BestUnit(zNew, "Length") << G4endl;
////			G4cout << "    actual_sigma " << G4BestUnit(actual_sigma, "Length") << G4endl;
////			G4cout << "    minimizeme " << minimizeme << G4endl;
////		}
//	}
//
//	// No z new was found, should not occur!
//	G4cout << "\nEVENT: "
//			<< G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID()
//			<< G4endl;
//	G4cout << "    z " << G4BestUnit(z, "Length") << G4endl;
//	G4cout << "    sigma " << G4BestUnit(sigma, "Length") << G4endl;
//
//	G4Exception("Digitizer::CalcZfromSigma()", "MyCode0004", FatalException,
//			"Cannot calculate hit z-position for initial charge cloud simulation. Decrease sigma0.");
//	return 0;
//}

double Digitizer::getD()
{
    // Diffusion constant in cm^2/s
    // mu: mobility in cm^2/sV
    // T: temp in K
    double kb_K_e = 8.6173e-5;
    return kb_K_e * fTemperatur * fMu;
}

void Digitizer::PrintSettings()
{
	G4cout << "Digitization settings: " << G4endl;
	if (fCalcChargeCloud)
	{
		G4cout << "  Calculate charge cloud: yes\t" << G4endl;
		G4cout << "    Bias\t" << fBias << G4endl;
		G4cout << "    Depletion voltage\t" << fVdep << G4endl;
		G4cout << "    Temperature\t" << fTemperatur << G4endl;
		G4cout << "    Init. Sigma\t" << G4BestUnit(fSigma0, "Length") << G4endl;
		G4cout << "    Sigma Corr. Factor\t" << fSigmaCC << G4endl;
		if (fNtype)
			G4cout << "    Bulk type\tn" << G4endl;
		else
			G4cout << "    Bulk type\tp" << G4endl;
	}
	else
	G4cout << "  Calculate charge cloud: no\t" << G4endl;
	G4cout << "  Digitization parameters: " << G4endl;
	G4cout << "    Thickness\t" << G4BestUnit(fSensorThickness, "Length") << G4endl;
	G4cout << "    Number of columns\t" << fNcolumns << G4endl;
	G4cout << "    Column width\t" << G4BestUnit(fPixelPitchX, "Length") << G4endl;
	G4cout << "    Number of rows\t" << fNrows << G4endl;
	G4cout << "    Row width\t" << G4BestUnit(fPixelPitchY, "Length") << G4endl;
	G4cout << "    Threshold\t" << fThreshold << G4endl;
	G4cout << "    Noise\t" << fNoise << G4endl;
	G4cout << "    Trigger\t" << fTriggerHits << G4endl;
	G4cout << "    E-H energy electrons\t" << fEHPerChargeElectrons << " eV" << G4endl;
	G4cout << "    E-H energy photons\t" << fEHPerChargePhotons << " eV" << G4endl;
}

void Digitizer::SetEnergyPerCharge(const G4double& energyPerCharge)
{
	fEHPerChargeElectrons = energyPerCharge;
	G4cout << "Set energy per e-h pair to "
			<< G4BestUnit(energyPerCharge, "Energy") << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetThreshold(const G4int& threshold)
{
	fThreshold = threshold;
	G4cout << "Set threshold to " << threshold << " electrons" << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetNoise(const G4int& noise)
{
	fNoise = noise;
	G4cout << "Set noise to " << noise << " electrons" << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetBias(const G4double& bias)
{
	fBias = bias;
	G4cout << "Set bias voltage to " << bias << " volt" << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetDepletion(const G4double& voltage)
{
	fVdep = voltage;
	std::cout<<"Set depletion voltage to " << voltage << " volt" << std::endl;
	G4cout << "Set depletion voltage to " << voltage << " volt" << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetTemperature(const G4double& temperature)
{
	fTemperatur = temperature;
	G4cout << "Set temperature to " << temperature << " Kelvin" << G4endl; // FIXME: G4cout + MT not working
}
void Digitizer::SetSensorZdirection(const bool& direction)
{
	fReadOutDirection = direction;
	if (direction)
		G4cout << "Set read out electronics behind sensor" << G4endl; // FIXME: G4cout + MT not working
	else
		G4cout << "Set read out electronics before sensor" << G4endl; // FIXME: G4cout + MT not working
}
void Digitizer::SetInitChargeCloudSigma(const G4double& sigma0) // the initial charge cloud is not zero due to repulsion
{
	fSigma0 = sigma0;
	std::cout<<"Set the initial charge cloud sigma t " << G4BestUnit(fSigma0, "Length") << std::endl;
	G4cout << "Set the initial charge cloud sigma to "
			<< G4BestUnit(fSigma0, "Length") << G4endl; // FIXME: G4cout + MT not working
}
void Digitizer::SetChargeCloudSigmaCorrection(const G4double& sigmaCC) // the sigma is higher due to repulsion, so correct sigma with factor > 0; very simple approximation; for further info see NIMA 606 (2009) 508-516
{
	if (sigmaCC > 0.5)
	{
		fSigmaCC = sigmaCC;
		G4cout << "Set charge cloud sigma correction factor to " << sigmaCC
				<< G4endl;  // FIXME: G4cout + MT not working
	}
}
void Digitizer::SetTrigger(const bool& triggerHits)
{
	fTriggerHits = triggerHits;
	if (triggerHits)
		G4cout << "Set trigger functionality to ON " << G4endl; // FIXME: G4cout + MT not working
	else
		G4cout << "Set trigger functionality to OFF " << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetNtype(const bool& tNtype)
{
	fNtype = tNtype;
	if (fNtype)
		G4cout << "Set n-type bulk" << G4endl; // FIXME: G4cout + MT not working
	else
		G4cout << "Set p-type bulk" << G4endl; // FIXME: G4cout + MT not working
}

void Digitizer::SetRepulsion(const bool& tRepulsion)
{
	fRepulsion = tRepulsion;
	if (fRepulsion)
		G4cout << "Take repulsion into account" << G4endl; // FIXME: G4cout + MT not working
	else
		G4cout << "Diffusion only" << G4endl; // FIXME: G4cout + MT not working
}

double Digitizer::coth(const double& x)
{
	return cosh(x) / sinh(x);
}

// Private
double Digitizer::getGapEnergy(const double& temperature)
{
	/* Temperature dependence of semiconductor band gaps
	 K. P. O Donnell and X. Chen
	 University of Strathclyde, Glasgow, G4 ONG Scotland, United Kingdom
	 (Received 5 November 1990; accepted for publication 27 March 1991 */
	double Eg0 = 1.170;  // eV
	double S = 1.49;  // unitless
	double hw = 25.5;  // meV
	double kbT = 8.61733035e-2;  // meV/K

	return Eg0 - S * hw * (coth(hw / (2. * kbT * temperature)) - 1.) * 0.001;
}

double Digitizer::getEHenergyElectrons(const double& temperature)
{
	/* PHYSICAL REVIEW VOLUME 140, NUM B ER 6A i 3 DECEM HER 1965
	 Average Energy Expended Per Ionized Electron-Hole Pair in Silicon and
	 Germanium as a Function of Temperature*
	 F. E. EMERvt sNn T. A. RAssoN
	 Department of Electricat Eragieeerilg, ifice UNieersity, Hofsstort, Texas
	 (Received 19 July 1965)
	 */
	double ehe = 2.2 * getGapEnergy(temperature)
			+ 1.08 * pow(getGapEnergy(temperature), 3. / 2.)
					* exp(11.2 * getGapEnergy(temperature) / temperature);
	return ehe;
}

double Digitizer::getEHenergy(const double& temperature)
{
	/* NUCLEAR INSTRUMENTS AND METHODS 59 (1968) 45-55
	 * ACCURATE DETERMINATION OF THE IONIZATION ENERGY IN SEMICONDUCTOR DETECTORS
	 * Newest paper with same e-h pair energy for electrons and photons (as another 199x paper
	 * suggests) and linear behavior between T and e-h. Fit not mentioned in paper, thus
	 * data digitized and fitted to come up with the following formular.
	 */
	return -6.72E-04 * temperature + 3.87;
}



double Digitizer::getEHenergyPhotons(const double& temperature)
{
	/* A lot of measurements exists with quite different values and
	 * theories. One can generally assume that systematic errors reduced
	 * in recent times making actual result more reliable. The values for
	 * a and w_300 have been chosen in a way that the Fe-55 plot in
	 * "Electron–hole pair creation energy and Fano factor temperature dependence in silicon, M.N. Mazziotta"
	 * is matched with the gab(temperature) description chosen in this code.
	 * This function is nevertheless hardly used since photo electric effect leads to electrons with other e-h formular.
	 */
	double a = 1.6;  // chose to match data
	double w_300 = 3.65;  // eV at 300 K
	double ehe = w_300 + a * (getGapEnergy(temperature) - getGapEnergy(300));
	G4cout << " getEHenergyPhotons " << G4BestUnit(ehe, "Energy") << G4endl;
	return ehe;
}

double Digitizer::getSigmaPhotons(const double& energy)
{
	/* Validity of spherical approximations of initial charge cloud shape in
	 silicon detectors
	 Cheng Xu, MatsDanielsson, HansBornefalk
	 Department of Physics, RoyalInstituteofTechnology, Stockholm, Sweden
	 */
	// G4cout << " getSigmaPhotons " << G4BestUnit(0.0044 * pow(energy / keV, 1.75), "Length") << G4endl;
	return 0.0044 * pow(energy / keV, 1.75) * um;
}
