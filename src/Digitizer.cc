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

#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "PixelROWorld.hh"

Digitizer::Digitizer(G4String name) :
		G4VDigitizerModule(name),
		fCalcChargeCloud(true),
		fReadOutDirection(false),
		fEnergyPerCharge(3.74),
		fThreshold(3000),  // std. IBL detector
		fNoise(130),  // std. IBL detector
		fTemperatur(298),  // 25 C
		fBias(70.), fPixelDigitsCollection(0)

{
	G4String colName = "PixelDigitsCollection";
	collectionName.push_back(colName);

	digiMessenger = new DigitizerMessenger(this);

	// get pixel detector info needed for digitization
	DetectorConstruction const* detector = dynamic_cast<DetectorConstruction const*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	PixelROWorld* pWorld = dynamic_cast<PixelROWorld*>(detector->GetParallelWorld(0));
	fSensorThickness = pWorld->GetSensorThickness();
	fPixelPitchX = pWorld->GetPixelPitchX();
	fPixelPitchY = pWorld->GetPixelPitchY();
	fNcolumns = pWorld->GetNcolumns();
	fNrows = pWorld->GetNrows();

	PrintSettings();
}

Digitizer::~Digitizer()
{
	delete digiMessenger;
}

void Digitizer::Digitize()
{
	G4DigiManager* digiMan = G4DigiManager::GetDMpointer();

	// Create a temporary actual event digits collection, to be able to remove hits below threshold, G4TDigiCollection does not support deleting entries ?!
	PixelDigitsCollection* actualPixelDigitsCollection = new PixelDigitsCollection("PixelDigitizer", "PixelDigitsCollection");

	// Get actual event hits collection
	G4int pixelDetectorHCID = G4SDManager::GetSDMpointer()->GetCollectionID("PixelDetektor/PixelHitCollection");
	DetHitsMap* hitsCollection = const_cast<DetHitsMap*>(static_cast<const DetHitsMap*>(digiMan->GetHitsCollection(pixelDetectorHCID)));  // a beautiful line to show what can go wrong in C++ frameworks when too less time is spend on the API...
	if (!hitsCollection)
		G4Exception("Digitizer::Digitize()", "Cannot access pixel hits collection", FatalException, "");

//	std::cout<<"\nEVENT: "<<G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID()<<std::endl;

	// This is a loop with an inner loop; speed wise not the best solution, but the geant4 examples are even worse and loop all hits + digits (1 pixel = 1 digit...), here the least amount of hits / digits were created and are looped
	for (std::map<G4int, DetHit*>::const_iterator it = hitsCollection->GetMap()->begin(); it != hitsCollection->GetMap()->end(); ++it) {
		if (it->second->GetVolumeIdX() == -1)  // speed up, do not loop remaining and all empty DetHits
			break;
		AddHitToDigits(it, actualPixelDigitsCollection);
	}

	// Create the result actual event digits colection
	fPixelDigitsCollection = new PixelDigitsCollection("PixelDigitizer", "PixelDigitsCollection");

	// Apply noise and threshold to all actual pixel digis
	for (G4int iDigi = 0; iDigi < actualPixelDigitsCollection->entries(); ++iDigi) {
		(*actualPixelDigitsCollection)[iDigi]->SetCharge(G4RandGauss::shoot((*actualPixelDigitsCollection)[iDigi]->GetCharge(), fNoise));  // add gaussian noise to the charge
		if ((*actualPixelDigitsCollection)[iDigi]->GetCharge() >= fThreshold)
			fPixelDigitsCollection->insert(new PixelDigi(*(*actualPixelDigitsCollection)[iDigi]));
	}

	StoreDigiCollection(fPixelDigitsCollection);
}

void Digitizer::AddHitToDigits(std::map<G4int, DetHit*>::const_iterator iHit, PixelDigitsCollection* digits)
{
	// Set the total charge per digi pixel by summing all hits within the different pixel volumes (= 2D charge histograming), if activated calculate charge sharing due to diffusion
	G4int column = iHit->second->GetVolumeIdX();
	G4int row = iHit->second->GetVolumeIdY();
	G4double charge = iHit->second->GetEdep() / fEnergyPerCharge / eV;  // charge in electrons
	G4ThreeVector position = iHit->second->GetPosition();

	double z = 0;  // z position in sensor [0..thickness]
	double fraction = 0;  // fraction of the total charge for the actual pixel
	double minFraction = 1e-5;
	double totalFraction = 0;

	if (fCalcChargeCloud) {  // distribute the deposited charge into neighboring pixels

		if (fReadOutDirection)  // calculate drift/read out electronics direction
			z = fSensorThickness / 2. + position[2];  // RO side after sensor
		else
			z = fSensorThickness / 2. - position[2];  // RO side before sensor

//		if (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() == 11637)
//			std::cout << column<<" "<<row<<" "<<charge<<"\n";

		if (z == 0){  // special case. charge does not travel at all
			AddChargeToDigits(column, row , charge, digits);
			return;
		}

		// run time optimized loops using an abort condition utilizing that the charge sharing always decreases for increased distance to seed pixel and the fact that the total charge fraction sum is 1
		for (int iColumn = 0; column + iColumn < fNcolumns; ++iColumn) {  // calc charge in pixels in column direction
			if (totalFraction == 1. || CalcChargeFraction(position[0], position[1], z, fPixelPitchX, fPixelPitchY, fBias, iColumn, 0, fTemperatur) < minFraction)  // omit row loop if charge fraction is already too low for seed row (=0)
				break;
			for (int iRow = 0; iRow < fNrows; ++iRow) {  // calc charge in pixels in row direction
				fraction = CalcChargeFraction(position[0], position[1], z, fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow, fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction)  //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
				AddChargeToDigits(column + iColumn, row + iRow, charge * fraction, digits);
				if (totalFraction == 1.)  // abort if all charge already distributed
					break;
			}
			for (int iRow = -1; row + iRow >= 0; --iRow) {  // calc charge in pixels in -row direction
				fraction = CalcChargeFraction(position[0], position[1], z, fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow, fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction)  //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
				AddChargeToDigits(column + iColumn, row + iRow, charge * fraction, digits);
				if (totalFraction == 1.)  // abort if all charge already distributed
					break;
			}
		}
		for (int iColumn = -1; column + iColumn >= 0; --iColumn) {  // calc charge in pixels in -column direction
			if (totalFraction == 1. || CalcChargeFraction(position[0], position[1], z, fPixelPitchX, fPixelPitchY, fBias, iColumn, 0, fTemperatur) < minFraction)  // omit row loop if charge fraction is already too low for seed row (=0)
				break;
			for (int iRow = 0; iRow < fNrows; ++iRow) {  // calc charge in pixels in row direction
				fraction = CalcChargeFraction(position[0], position[1], z, fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow, fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction)  //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
				AddChargeToDigits(column + iColumn, row + iRow, charge * fraction, digits);
				if (totalFraction == 1.)  // abort if all charge already distributed
					break;
			}
			for (int iRow = -1; row + iRow >= 0; --iRow) {  // calc charge in pixels in -row direction
				fraction = CalcChargeFraction(position[0], position[1], z, fPixelPitchX, fPixelPitchY, fBias, iColumn, iRow, fTemperatur);
				totalFraction += fraction;
				if (fraction < minFraction)  //  abort loop if fraction is too small, next pixel have even smaller fraction
					break;
				AddChargeToDigits(column + iColumn, row + iRow, charge * fraction, digits);
				if (totalFraction == 1.)  // abort if all charge already distributed
					break;
			}
		}
	}
	else
		AddChargeToDigits(column, row, charge, digits);
}

void Digitizer::AddChargeToDigits(const int& column, const int& row, const double& charge, PixelDigitsCollection* digits)
{
// Runtime ugly function, a two key hash map would be nice...
//	if (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() == 11637)
//		std::cout << "    AddChargeToDigits col/row/charge=" <<column<<"/"<<row<<"/"<<charge<<"\n";
	bool digitExists = false;
	for (G4int iDigi = 0; iDigi < digits->entries(); ++iDigi) {  // go through all already created digis
		if (column == (*digits)[iDigi]->GetColumn() && row == (*digits)[iDigi]->GetRow()) {
			(*digits)[iDigi]->Add(charge);
			digitExists = true;
			break;
		}
	}

	if (!digitExists) {  // pixel digi does not exist, thus create new
		PixelDigi* digi = new PixelDigi(column, row, charge);
		digits->insert(digi);
	}
}

double Digitizer::CalcChargeFraction(const double& x, const double& y, const double& z, const double& x_pitch, const double& y_pitch, const double& voltage, const int& x_pixel_offset, const int& y_pixel_offset, const double& temperature)
{
	/* Calculates the fraction of charge [0, 1] within one rectangular pixel volume when diffusion is considered. The calculation is done within the local pixel coordinate system, with the origin [x_pitch / 2, y_pitch / 2, 0]
	 x, y : position where the charge fraction has to be measured
	 x_pitch, y_pitch : pixel dimensions in x and y
	 x_pixel_offset, y_pixel_offset : pixel index relative to the center pixel where the charge fraction has to be calculated
	 voltage : the applied voltage
	 temperature : the temperature in Kelvin
	 */

	double sigma = CalcSigmaDiffusion(z, voltage, temperature);

//	if (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() == 11637){
//		std::cout << "    CalcChargeFraction " <<"\n";
//		std::cout << "      x/y/z " << G4BestUnit(x, "Length")<<"/"<< G4BestUnit(y, "Length")<<"/"<<G4BestUnit(z, "Length")<<"\n";
//		std::cout << "        "<<G4BestUnit(x, "Length")<<" "<<G4BestUnit(y, "Length")<<" "<<G4BestUnit(z, "Length")<<" "<<G4BestUnit(x_pitch, "Length")<<" "<<G4BestUnit(y_pitch, "Length")<<" "<<voltage<<" "<<x_pixel_offset<<" "<<y_pixel_offset<<" "<<temperature<<"\n";
//		std::cout << "      sigma " << G4BestUnit(sigma, "Length")<<"\n";
//	}

	if (sigma == 0)
		return 1.;
	return CalcBivarianteNormalCDFWithLimits(x_pitch * (x_pixel_offset - 1. / 2.), x_pitch * (x_pixel_offset + 1. / 2.), y_pitch * (y_pixel_offset - 1. / 2.), y_pitch * (y_pixel_offset + 1. / 2.), x, y, sigma);
}

double Digitizer::CalcBivarianteNormalCDFWithLimits(const double& a1, const double& a2, const double& b1, const double& b2, const double& mu1, const double& mu2, const double& sigma)
{
// Calculates the integral of the bivariante normal distribution between x = [a1, a2], y = [b1, b2]. The normal distribution has two mu: mu1, mu2 but only one common sigma.
	return 1. / 4. * (erf((a2 - mu1) / std::sqrt(2 * sigma * sigma)) - erf((a1 - mu1) / std::sqrt(2 * sigma * sigma))) * (erf((b2 - mu2) / std::sqrt(2 * sigma * sigma)) - erf((b1 - mu2) / std::sqrt(2 * sigma * sigma)));
}

double Digitizer::CalcSigmaDiffusion(const double& length, const double& voltage, const double& temperature)
{
	/* Calculates the sigma of the diffusion according to Einsteins equation.
	 length : the drift length
	 voltage : the applied voltage
	 temperature : the temperature in Kelvin
	 */
	const double kb_K_e = 8.6173e-5;  // Boltzmann Constant * Kelvin / elemetary charge
	return length * std::sqrt(2 * temperature / voltage * kb_K_e);
}

void Digitizer::PrintSettings()
{
	G4cout << "Digitization settings: " << G4endl;
	G4cout << "  Calculate charge cloud\t" << fCalcChargeCloud << G4endl;
	G4cout << "  Digitization parameters: " << G4endl;
	G4cout << "    Thickness\t" << G4BestUnit(fSensorThickness, "Length") << G4endl;
	G4cout << "    Number of columns\t" << fNcolumns << G4endl;
	G4cout << "    Column width\t" << G4BestUnit(fPixelPitchX, "Length") << G4endl;
	G4cout << "    Number of rows\t" << fNrows << G4endl;
	G4cout << "    Row width\t" << G4BestUnit(fPixelPitchY, "Length") << G4endl;
}

void Digitizer::SetEnergyPerCharge(const G4double& energyPerCharge)
{
	fEnergyPerCharge = energyPerCharge;
}

void Digitizer::SetThreshold(const G4int& threshold)
{
	fThreshold = threshold;
}

void Digitizer::SetNoise(const G4int& noise)
{
	fNoise = noise;
}

