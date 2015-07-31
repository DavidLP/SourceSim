#include "DigitizerMessenger.hh"
#include "Digitizer.hh"

#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

DigitizerMessenger::DigitizerMessenger(Digitizer* digitizer)
		: G4UImessenger(), fDigitizer(digitizer)
{
	fDigiSetupDir = new G4UIdirectory("/digitizer/");
	fDigiSetupDir->SetGuidance("UI commands specific to the setup the digitization of this simulation.");

	fThresholdCmd = new G4UIcmdWithAnInteger("/digitizer/threshold", this);
	fThresholdCmd->SetGuidance("Threshold in electrons to create a pixel digi");
	fThresholdCmd->SetParameterName("choice", true);
	fThresholdCmd->SetDefaultValue((G4int) 2000);
	fThresholdCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fNoiseCmd = new G4UIcmdWithAnInteger("/digitizer/noise", this);
	fNoiseCmd->SetGuidance("Noise in electrons to create a pixel digi");
	fNoiseCmd->SetParameterName("choice", true);
	fNoiseCmd->SetDefaultValue((G4int) 2000);
	fNoiseCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fBiasCmd = new G4UIcmdWithADouble("/digitizer/bias", this);
	fBiasCmd->SetGuidance("Sensor bias in volt");
	fBiasCmd->SetParameterName("choice", true);
	fBiasCmd->SetDefaultValue(60.);
	fBiasCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fTemperatureCmd = new G4UIcmdWithADouble("/digitizer/temperature", this);
	fTemperatureCmd->SetGuidance("Sensor temperature in Kelvin");
	fTemperatureCmd->SetParameterName("choice", true);
	fTemperatureCmd->SetDefaultValue(300.);
	fTemperatureCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fDriftDirectionCmd = new G4UIcmdWithAnInteger("/digitizer/driftdirection", this);
	fDriftDirectionCmd->SetGuidance("Z-Drift direction of the electrons towards readout electrode");
	fDriftDirectionCmd->SetParameterName("choice", true);
	fDriftDirectionCmd->SetDefaultValue((G4int) 0);
	fDriftDirectionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fInitChargeCloudCmd = new G4UIcmdWithADoubleAndUnit("/digitizer/initsigmacc", this);
	fInitChargeCloudCmd->SetGuidance("Initial charge cloud sigma");
	fInitChargeCloudCmd->SetParameterName("initcc", true);
	fInitChargeCloudCmd->SetUnitCategory("Length");
	fInitChargeCloudCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fCorrChargeCloudCmd = new G4UIcmdWithADouble("/digitizer/cccorrection", this);
	fCorrChargeCloudCmd->SetGuidance("Charge cloud sigma correction factor");
	fCorrChargeCloudCmd->SetParameterName("choice", true);
	fCorrChargeCloudCmd->SetDefaultValue(1.);
	fCorrChargeCloudCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	fTriggerCmd = new G4UIcmdWithAnInteger("/digitizer/trigger", this);
	fTriggerCmd->SetGuidance("Enable the triggering of detector hits");
	fTriggerCmd->SetParameterName("choice", true);
	fTriggerCmd->SetDefaultValue((G4int) 0);
	fTriggerCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DigitizerMessenger::~DigitizerMessenger()
{
	delete fDigiSetupDir;
	delete fThresholdCmd;
	delete fNoiseCmd;
	delete fBiasCmd;
	delete fTemperatureCmd;
	delete fInitChargeCloudCmd;
	delete fCorrChargeCloudCmd;
	delete fDriftDirectionCmd;
}

void DigitizerMessenger::SetNewValue(G4UIcommand * command, G4String newValue)
{
	if (command == fThresholdCmd) {
		fDigitizer->SetThreshold(fThresholdCmd->GetNewIntValue(newValue));
	}
	if (command == fNoiseCmd) {
		fDigitizer->SetNoise(fThresholdCmd->GetNewIntValue(newValue));
	}
	if (command == fBiasCmd) {
		fDigitizer->SetBias(fBiasCmd->GetNewDoubleValue(newValue));
	}
	if (command == fTemperatureCmd) {
		fDigitizer->SetTemperature(fTemperatureCmd->GetNewDoubleValue(newValue));
	}
	if (command == fInitChargeCloudCmd) {
		fDigitizer->SetInitChargeCloudSigma(fInitChargeCloudCmd->GetNewDoubleValue(newValue));
	}
	if (command == fCorrChargeCloudCmd) {
		fDigitizer->SetChargeCloudSigmaCorrection(fCorrChargeCloudCmd->GetNewDoubleValue(newValue));
	}
	if (command == fDriftDirectionCmd) {
		if (fDriftDirectionCmd->GetNewIntValue(newValue) > 0)
			fDigitizer->SetSensorZdirection(true);
		else
			fDigitizer->SetSensorZdirection(false);
	}
	if (command == fTriggerCmd) {
		if (fTriggerCmd->GetNewIntValue(newValue) > 0)
			fDigitizer->SetTrigger(true);
		else
			fDigitizer->SetTrigger(false);
	}
}

