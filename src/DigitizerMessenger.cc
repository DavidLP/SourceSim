#include "DigitizerMessenger.hh"

#include "Digitizer.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAnInteger.hh"

DigitizerMessenger::DigitizerMessenger(Digitizer* digitizer)
		: fDigitizer(digitizer)
{
	fThresholdCmd = new G4UIcmdWithAnInteger("/digitizer/threshold", this);
	fThresholdCmd->SetGuidance("Threshold in electrons to create a pixel digi");
	fThresholdCmd->SetParameterName("choice", true);
	fThresholdCmd->SetDefaultValue((G4int) 2000);
	fThresholdCmd->SetRange("threshold >=0.");
	fThresholdCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fNoiseCmd = new G4UIcmdWithAnInteger("/digitizer/noise", this);
	fNoiseCmd->SetGuidance("Noise in electrons to create a pixel digi");
	fNoiseCmd->SetParameterName("choice", true);
	fNoiseCmd->SetDefaultValue((G4int) 2000);
	fNoiseCmd->SetRange("noise >=0.");
	fNoiseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DigitizerMessenger::~DigitizerMessenger()
{
	delete fThresholdCmd;
	delete fNoiseCmd;
}

void DigitizerMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{
	if (command == fThresholdCmd) {
		fDigitizer->SetThreshold(fThresholdCmd->GetNewIntValue(newValue));
	}
	if (command == fNoiseCmd) {
		fDigitizer->SetThreshold(fThresholdCmd->GetNewIntValue(newValue));
	}
}

