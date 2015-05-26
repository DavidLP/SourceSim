#ifndef Digitizer_h
#define Digitizer_h 1

#include "G4VDigitizerModule.hh"
#include "globals.hh"
#include "PixelDigi.hh"

class DigitizerMessenger;

class Digitizer: public G4VDigitizerModule {
	public:

		Digitizer(G4String name);
		~Digitizer();

		void Digitize();
		void SetEnergyPerCharge(const G4double&);  // energy per charge in electrons per electronvolt, e.g. silicon 3.61-3.74
		void SetThreshold(const G4int&);  // threshold in electrons
		void SetNoise(const G4int&);  // sigma of gaussian noise in electrons

	private:

		G4double fEnergyPerCharge;
		G4int fThreshold;
		G4int fNoise;

		PixelDigitsCollection* fPixelDigitsCollection;

		DigitizerMessenger* digiMessenger;

};

#endif

