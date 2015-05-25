#ifndef GammaRayTel_h
#define GammaRayTel_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class Digitizer;
class G4UIcmdWithAnInteger;

class DigitizerMessenger: public G4UImessenger {
	public:
		DigitizerMessenger(Digitizer*);
		~DigitizerMessenger();

		void SetNewValue(G4UIcommand*, G4String);

	private:
		Digitizer* fDigitizer;
		G4UIcmdWithAnInteger* fThresholdCmd;
		G4UIcmdWithAnInteger* fNoiseCmd;

};

#endif

