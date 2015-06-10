#include "DigitizerMessenger.hh"

#include "Digitizer.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"

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

	fBiasCmd = new G4UIcmdWithADouble("/digitizer/bias", this);
	fBiasCmd->SetGuidance("Sensor bias in volt");
	fBiasCmd->SetParameterName("choice", true);
	fBiasCmd->SetDefaultValue((G4int) 60);
	fBiasCmd->SetRange("bias >=0.");
	fBiasCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fTemperatureCmd = new G4UIcmdWithADouble("/digitizer/temperature", this);
	fTemperatureCmd->SetGuidance("Sensor temperature in Kelvin");
	fTemperatureCmd->SetParameterName("choice", true);
	fTemperatureCmd->SetDefaultValue((G4int) 300);
	fTemperatureCmd->SetRange("temperature >=0.");
	fTemperatureCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDriftDirectionCmd = new G4UIcmdWithAnInteger("/digitizer/driftdirection", this);
	fDriftDirectionCmd->SetGuidance("Z-Drift direction of the electrons towards readout electrode");
	fDriftDirectionCmd->SetParameterName("choice", true);
	fDriftDirectionCmd->SetDefaultValue((G4int) 0);
	fDriftDirectionCmd->SetRange("driftdirection >=0.");
	fDriftDirectionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DigitizerMessenger::~DigitizerMessenger()
{
	delete fThresholdCmd;
	delete fNoiseCmd;
	delete fBiasCmd;
	delete fTemperatureCmd;
	delete fDriftDirectionCmd;
}

void DigitizerMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{
	if (command == fThresholdCmd) {
		fDigitizer->SetThreshold(fThresholdCmd->GetNewIntValue(newValue));
	}
	if (command == fNoiseCmd) {
		fDigitizer->SetThreshold(fThresholdCmd->GetNewIntValue(newValue));
	}
	if (command == fBiasCmd) {
		fDigitizer->SetBias(fBiasCmd->GetNewDoubleValue(newValue));
	}
	if (command == fTemperatureCmd) {
		fDigitizer->SetTemperature(fTemperatureCmd->GetNewDoubleValue(newValue));
	}
	if (command == fDriftDirectionCmd) {
		if (fDriftDirectionCmd->GetNewIntValue(newValue) > 0)
			fDigitizer->SetSensorZdirection(true);
		else
			fDigitizer->SetSensorZdirection(false);
	}
}

