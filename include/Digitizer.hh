#ifndef Digitizer_h
#define Digitizer_h 1

#include "G4VDigitizerModule.hh"
#include "globals.hh"
#include "PixelDigi.hh"

class DigitizerMessenger;
class DetHit;

class Digitizer: public G4VDigitizerModule {
	public:

		Digitizer(G4String name);
		~Digitizer();

		void Digitize();

		void SetEnergyPerCharge(const G4double&);  // energy per charge in electrons per electronvolt, e.g. silicon 3.61-3.74
		void SetThreshold(const G4int&);  // threshold in electrons
		void SetNoise(const G4int&);  // sigma of gaussian noise in electrons
		void SetBias(const G4double&);  // sensor bias [V]
		void SetDepletion(const G4double&);  // sensor depletion voltage [V]
		void SetTemperature(const G4double&);  // sensor temperatur [K]
		void SetSensorZdirection(const bool&);  // electron z-drift direction, always towards readout electrode; false: in z-direction
		void SetInitChargeCloudSigma(const G4double&); // the initial spacing of the charge cloud sigma [um]
		void SetChargeCloudSigmaCorrection(const G4double&); // the initial spacing of the charge cloud sigma [um]
		void SetTrigger(const bool&); // the detector hits are only stored if a trigger exists in the event
		void SetNtype(const bool&); // the sensor bulk type
		void SetRepulsion(const bool& tRepulsion); // calculate sigma charge cloud including repulsion

		void PrintSettings();

	private:
		void AddHitToDigits(std::map<G4int, DetHit*>::const_iterator iHit, PixelDigitsCollection* digits);
		void AddChargeToDigits(const int& column, const int& row, const double& charge, PixelDigitsCollection* digits);

		double CalcChargeFraction(const double& x, const double& y, const double& z, const double& charge, const double& x_pitch, const double& y_pitch, const double& voltage, const int& x_pixel_offset, const int& y_pixel_offset, const double& temperature);
		double CalcSigmaDiffusion(const double& length, const double& voltage, const double& temperature, const double& charge);
		double CalcDriftTime(const double& length, const double& voltage);
		double CalcBivarianteNormalCDFWithLimits(const double& a1, const double& a2, const double& b1, const double& b2, const double& mu1, const double& mu2, const double& sigma);
		double CalcZfromSigma(const double& z, const double& sigma);

		double coth(const double& x);
		double getGapEnergy(const double& temperature);
		double getEHenergy(const double& temperature);
		double getEHenergyElectrons(const double& temperature);
		double getEHenergyPhotons(const double& temperature);
		double getSigmaPhotons(const double& energy);
		double getD();
		//Digitization settings
		bool fCalcChargeCloud;

		//Digitization parameters
		bool fReadOutDirection; // false: readout side more towards z (behind sensor)
		G4double fEHPerChargeElectrons;  // charge per e-h pair for electrons
		G4double fEHPerChargePhotons;  // charge per e-h pair for photons
		G4int fThreshold;  // detection thresholdl [e]
		G4int fNoise;  // gaussian noise sigma [e]
		G4double fTemperatur;  // [Kelvin]
		G4double fBias;  // sensor bias voltage [V]
		G4double fVdep;  // sensor depletion voltage [V]
		G4double fSigma0; // initial charge cloud sigma [um]
		G4double fSigmaCC; // correction factor for charge cloud sigma
		bool fNtype;  // detector bulk type
		bool fRepulsion;  // handle repulsion of charge carriers
		bool fTriggerHits;  // create hits only if there are hits in the trigger volume

		//Constant variables that cannot be changed yet between runs, yet
		G4double fSensorThickness;  // [um]
		G4double fPixelPitchX;  // [um]
		G4double fPixelPitchY;  // [um]
		G4int fNcolumns;
		G4int fNrows;
		G4double fMu; // Electron mobility in silicon [um2 / us /V]

		PixelDigitsCollection* fPixelDigitsCollection;

		DigitizerMessenger* digiMessenger;

		G4int fTriggerHCID;
};

#endif

