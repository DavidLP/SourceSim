#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction * Det) :
		G4UImessenger(), fDetector(Det)
{
	defineCommands();
}

DetectorMessenger::~DetectorMessenger()
{
	delete fSetupDir;
	delete fWorldDir;
	delete fSensorDir;
	delete fTriggerDir;
	delete fCollDir;
	delete fSourceShieldDir;
	delete fShieldDir;

	delete fWorldMaterCmd;
	delete fWorldZCmd;
	delete fWorldXYCmd;

	delete fSensorMaterCmd;
	delete fSensorThickCmd;
	delete fSensorSizXCmd;
	delete fSensorSizYCmd;
	delete fSensorPosCmd;
	delete fSensorRotCmd;

	delete fTriggerMaterCmd;
	delete fTriggerThickCmd;
	delete fTriggerSizXYCmd;
	delete fTriggerPosCmd;

	delete fCollToggleCmd;
	delete fCollInnerMaterCmd;
	delete fCollOuterMaterCmd;
	delete fCollInnerRadiusCmd;
	delete fCollMiddleRadiusCmd;
	delete fCollOuterRadiusCmd;
	delete fCollThickCmd;
	delete fCollPosCmd;

	delete fSourceShieldToggleCmd;
	delete fSourceShieldMaterCmd;
	delete fSourceShieldThickCmd;
	delete fSourceShieldSizXYCmd;
	delete fSourceShieldPosCmd;

	delete fShieldToggleCmd;
	delete fShieldMaterCmd;
	delete fShieldThickCmd;
	delete fShieldSizXYCmd;
	delete fShieldPosCmd;

	delete fTestCmd;
}

void DetectorMessenger::defineCommands()
{
	fSetupDir = new G4UIdirectory("/setup/");
	fSetupDir->SetGuidance("UI commands specific to the setup of this simulation.");
	fWorldDir = new G4UIdirectory("/setup/world/");
	fWorldDir->SetGuidance("World construction commands");
	fSensorDir = new G4UIdirectory("/setup/sensor/");
	fSensorDir->SetGuidance("Sensor construction commands");
	fTriggerDir = new G4UIdirectory("/setup/trigger/");
	fTriggerDir->SetGuidance("Trigger construction commands");
	fCollDir = new G4UIdirectory("/setup/collimator/");
	fCollDir->SetGuidance("Collimator construction commands");
	fSourceShieldDir = new G4UIdirectory("/setup/sourceshield/");
	fSourceShieldDir->SetGuidance("Source shield construction commands");
	fShieldDir = new G4UIdirectory("/setup/shield/");
	fShieldDir->SetGuidance("Shield construction commands");

	fWorldMaterCmd = new G4UIcmdWithAString("/setup/world/material", this);
	fWorldMaterCmd->SetGuidance("Select Material of the World.");
	fWorldMaterCmd->SetParameterName("choice", false);
	fWorldMaterCmd->AvailableForStates(G4State_Idle);
	fWorldMaterCmd->SetToBeBroadcasted(false);

	fWorldZCmd = new G4UIcmdWithADoubleAndUnit("/setup/world/z", this);
	fWorldZCmd->SetGuidance("Set Z size of the World");
	fWorldZCmd->SetParameterName("WSizeZ", false);
	fWorldZCmd->SetRange("WSizeZ>0.");
	fWorldZCmd->SetUnitCategory("Length");
	fWorldZCmd->AvailableForStates(G4State_Idle);
	fWorldZCmd->SetToBeBroadcasted(false);

	fWorldXYCmd = new G4UIcmdWithADoubleAndUnit("/setup/world/xy", this);
	fWorldXYCmd->SetGuidance("Set sizeXY of the World");
	fWorldXYCmd->SetParameterName("WSizeXY", false);
	fWorldXYCmd->SetRange("WSizeXY>0.");
	fWorldXYCmd->SetUnitCategory("Length");
	fWorldXYCmd->AvailableForStates(G4State_Idle);
	fWorldXYCmd->SetToBeBroadcasted(false);

	fSensorMaterCmd = new G4UIcmdWithAString("/setup/sensor/material", this);
	fSensorMaterCmd->SetGuidance("Select Material of the sensor.");
	fSensorMaterCmd->SetParameterName("wchoice", false);
	fSensorMaterCmd->AvailableForStates(G4State_Idle);
	fSensorMaterCmd->SetToBeBroadcasted(false);

	fSensorSizXCmd = new G4UIcmdWithADoubleAndUnit("/setup/sensor/x", this);
	fSensorSizXCmd->SetGuidance("Set size X of the Sensor");
	fSensorSizXCmd->SetParameterName("SizeX", false);
	fSensorSizXCmd->SetRange("SizeX>0.");
	fSensorSizXCmd->SetUnitCategory("Length");
	fSensorSizXCmd->AvailableForStates(G4State_Idle);
	fSensorSizXCmd->SetToBeBroadcasted(false);

	fSensorSizYCmd = new G4UIcmdWithADoubleAndUnit("/setup/sensor/y", this);
	fSensorSizYCmd->SetGuidance("Set size Y of the Sensor");
	fSensorSizYCmd->SetParameterName("SizeY", false);
	fSensorSizYCmd->SetRange("SizeY>0.");
	fSensorSizYCmd->SetUnitCategory("Length");
	fSensorSizYCmd->AvailableForStates(G4State_Idle);
	fSensorSizYCmd->SetToBeBroadcasted(false);

	fSensorThickCmd = new G4UIcmdWithADoubleAndUnit("/setup/sensor/thickness", this);
	fSensorThickCmd->SetGuidance("Set Thickness of the Sensor");
	fSensorThickCmd->SetParameterName("SizeZ", false);
	fSensorThickCmd->SetRange("SizeZ>0.");
	fSensorThickCmd->SetUnitCategory("Length");
	fSensorThickCmd->AvailableForStates(G4State_Idle);
	fSensorThickCmd->SetToBeBroadcasted(false);

	fSensorPosCmd = new G4UIcmdWith3VectorAndUnit("/setup/sensor/pos", this);
	fSensorPosCmd->SetGuidance("Set pos. of the Sensor x y z");
	fSensorPosCmd->AvailableForStates(G4State_Idle);
	fSensorPosCmd->SetToBeBroadcasted(false);

	fSensorRotCmd = new G4UIcmdWithADoubleAndUnit("/setup/sensor/rot", this);
	fSensorRotCmd->SetGuidance("Set Rotation of the Sensor");
	fSensorRotCmd->SetUnitCategory("Angle");
	fSensorRotCmd->AvailableForStates(G4State_Idle);
	fSensorRotCmd->SetToBeBroadcasted(false);

	fTriggerToggleCmd = new G4UIcmdWithAnInteger("/setup/trigger/enable", this);
	fTriggerToggleCmd->SetGuidance("Enable/disable trigger.");
	fTriggerToggleCmd->SetParameterName("wchoice", false);
	fTriggerToggleCmd->AvailableForStates(G4State_Idle);
	fTriggerToggleCmd->SetToBeBroadcasted(false);

	fTriggerMaterCmd = new G4UIcmdWithAString("/setup/trigger/material", this);
	fTriggerMaterCmd->SetGuidance("Select Material of the Trigger.");
	fTriggerMaterCmd->SetParameterName("wchoice", false);
	fTriggerMaterCmd->AvailableForStates(G4State_Idle);
	fTriggerMaterCmd->SetToBeBroadcasted(false);

	fTriggerSizXYCmd = new G4UIcmdWithADoubleAndUnit("/setup/trigger/xy", this);
	fTriggerSizXYCmd->SetGuidance("Set sizeYZ of the Trigger");
	fTriggerSizXYCmd->SetParameterName("SizeXY", false);
	fTriggerSizXYCmd->SetRange("SizeXY>0.");
	fTriggerSizXYCmd->SetUnitCategory("Length");
	fTriggerSizXYCmd->AvailableForStates(G4State_Idle);
	fTriggerSizXYCmd->SetToBeBroadcasted(false);

	fTriggerThickCmd = new G4UIcmdWithADoubleAndUnit("/setup/trigger/thickness", this);
	fTriggerThickCmd->SetGuidance("Set Thickness of the Trigger");
	fTriggerThickCmd->SetParameterName("SizeZ", false);
	fTriggerThickCmd->SetRange("SizeZ>0.");
	fTriggerThickCmd->SetUnitCategory("Length");
	fTriggerThickCmd->AvailableForStates(G4State_Idle);
	fTriggerThickCmd->SetToBeBroadcasted(false);

	fTriggerPosCmd = new G4UIcmdWith3VectorAndUnit("/setup/trigger/pos", this);
	fTriggerPosCmd->SetGuidance("Set pos. of the Trigger x y z");
	fTriggerPosCmd->AvailableForStates(G4State_Idle);
	fTriggerPosCmd->SetToBeBroadcasted(false);

	fCollToggleCmd = new G4UIcmdWithAnInteger("/setup/collimator/enable", this);
	fCollToggleCmd->SetGuidance("Enable/disable collimator.");
	fCollToggleCmd->SetParameterName("wchoice", false);
	fCollToggleCmd->AvailableForStates(G4State_Idle);
	fCollToggleCmd->SetToBeBroadcasted(false);

	fCollInnerMaterCmd = new G4UIcmdWithAString("/setup/collimator/materialinner", this);
	fCollInnerMaterCmd->SetGuidance("Select Material of the inner collimator.");
	fCollInnerMaterCmd->SetParameterName("wchoice", false);
	fCollInnerMaterCmd->AvailableForStates(G4State_Idle);
	fCollInnerMaterCmd->SetToBeBroadcasted(false);

	fCollOuterMaterCmd = new G4UIcmdWithAString("/setup/collimator/materialouter", this);
	fCollOuterMaterCmd->SetGuidance("Select Material of the outer collimator.");
	fCollOuterMaterCmd->SetParameterName("wchoice", false);
	fCollOuterMaterCmd->AvailableForStates(G4State_Idle);
	fCollOuterMaterCmd->SetToBeBroadcasted(false);

	fCollInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/setup/collimator/radiusinner", this);
	fCollInnerRadiusCmd->SetGuidance("Set inner radius of the collimator");
	fCollInnerRadiusCmd->SetParameterName("SizeR", false);
	fCollInnerRadiusCmd->SetRange("SizeR>0.");
	fCollInnerRadiusCmd->SetUnitCategory("Length");
	fCollInnerRadiusCmd->AvailableForStates(G4State_Idle);
	fCollInnerRadiusCmd->SetToBeBroadcasted(false);

	fCollMiddleRadiusCmd = new G4UIcmdWithADoubleAndUnit("/setup/collimator/radiusmiddle", this);
	fCollMiddleRadiusCmd->SetGuidance("Set middle radius of the collimator");
	fCollMiddleRadiusCmd->SetParameterName("SizeR", false);
	fCollMiddleRadiusCmd->SetRange("SizeR>0.");
	fCollMiddleRadiusCmd->SetUnitCategory("Length");
	fCollMiddleRadiusCmd->AvailableForStates(G4State_Idle);
	fCollMiddleRadiusCmd->SetToBeBroadcasted(false);

	fCollOuterRadiusCmd = new G4UIcmdWithADoubleAndUnit("/setup/collimator/radiusouter", this);
	fCollOuterRadiusCmd->SetGuidance("Set outer radius of the collimator");
	fCollOuterRadiusCmd->SetParameterName("SizeR", false);
	fCollOuterRadiusCmd->SetRange("SizeR>0.");
	fCollOuterRadiusCmd->SetUnitCategory("Length");
	fCollOuterRadiusCmd->AvailableForStates(G4State_Idle);
	fCollOuterRadiusCmd->SetToBeBroadcasted(false);

	fCollThickCmd = new G4UIcmdWithADoubleAndUnit("/setup/collimator/thickness", this);
	fCollThickCmd->SetGuidance("Set thickness of the collimator");
	fCollThickCmd->SetParameterName("SizeZ", false);
	fCollThickCmd->SetRange("SizeZ>0.");
	fCollThickCmd->SetUnitCategory("Length");
	fCollThickCmd->AvailableForStates(G4State_Idle);
	fCollThickCmd->SetToBeBroadcasted(false);

	fCollPosCmd = new G4UIcmdWith3VectorAndUnit("/setup/collimator/pos", this);
	fCollPosCmd->SetGuidance("Set pos. of the collimator in x y z");
	fCollPosCmd->AvailableForStates(G4State_Idle);
	fCollPosCmd->SetToBeBroadcasted(false);

	fSourceShieldToggleCmd = new G4UIcmdWithAnInteger("/setup/sourceshield/enable", this);
	fSourceShieldToggleCmd->SetGuidance("Enable/disable Source Shield.");
	fSourceShieldToggleCmd->SetParameterName("wchoice", false);
	fSourceShieldToggleCmd->AvailableForStates(G4State_Idle);
	fSourceShieldToggleCmd->SetToBeBroadcasted(false);

	fSourceShieldMaterCmd = new G4UIcmdWithAString("/setup/sourceshield/material", this);
	fSourceShieldMaterCmd->SetGuidance("Select Material of the source shield.");
	fSourceShieldMaterCmd->SetParameterName("wchoice", false);
	fSourceShieldMaterCmd->AvailableForStates(G4State_Idle);
	fSourceShieldMaterCmd->SetToBeBroadcasted(false);

	fSourceShieldSizXYCmd = new G4UIcmdWithADoubleAndUnit("/setup/sourceshield/xy", this);
	fSourceShieldSizXYCmd->SetGuidance("Set sizeYZ of the Source Shield");
	fSourceShieldSizXYCmd->SetParameterName("SizeXY", false);
	fSourceShieldSizXYCmd->SetRange("SizeXY>0.");
	fSourceShieldSizXYCmd->SetUnitCategory("Length");
	fSourceShieldSizXYCmd->AvailableForStates(G4State_Idle);
	fSourceShieldSizXYCmd->SetToBeBroadcasted(false);

	fSourceShieldThickCmd = new G4UIcmdWithADoubleAndUnit("/setup/sourceshield/thickness", this);
	fSourceShieldThickCmd->SetGuidance("Set Thickness of the Source Shield");
	fSourceShieldThickCmd->SetParameterName("SizeZ", false);
	fSourceShieldThickCmd->SetRange("SizeZ>0.");
	fSourceShieldThickCmd->SetUnitCategory("Length");
	fSourceShieldThickCmd->AvailableForStates(G4State_Idle);
	fSourceShieldThickCmd->SetToBeBroadcasted(false);

	fSourceShieldPosCmd = new G4UIcmdWith3VectorAndUnit("/setup/sourceshield/pos", this);
	fSourceShieldPosCmd->SetGuidance("Set pos. of the Source Shield x y z");
	fSourceShieldPosCmd->AvailableForStates(G4State_Idle);
	fSourceShieldPosCmd->SetToBeBroadcasted(false);

	fShieldToggleCmd = new G4UIcmdWithAnInteger("/setup/shield/enable", this);
	fShieldToggleCmd->SetGuidance("Enable/disable shield.");
	fShieldToggleCmd->SetParameterName("wchoice", false);
	fShieldToggleCmd->AvailableForStates(G4State_Idle);
	fShieldToggleCmd->SetToBeBroadcasted(false);

	fShieldMaterCmd = new G4UIcmdWithAString("/setup/shield/material", this);
	fShieldMaterCmd->SetGuidance("Select Material of the shield.");
	fShieldMaterCmd->SetParameterName("wchoice", false);
	fShieldMaterCmd->AvailableForStates(G4State_Idle);
	fShieldMaterCmd->SetToBeBroadcasted(false);

	fShieldSizXYCmd = new G4UIcmdWithADoubleAndUnit("/setup/shield/xy", this);
	fShieldSizXYCmd->SetGuidance("Set sizeYZ of the shield");
	fShieldSizXYCmd->SetParameterName("SizeXY", false);
	fShieldSizXYCmd->SetRange("SizeXY>0.");
	fShieldSizXYCmd->SetUnitCategory("Length");
	fShieldSizXYCmd->AvailableForStates(G4State_Idle);
	fShieldSizXYCmd->SetToBeBroadcasted(false);

	fShieldThickCmd = new G4UIcmdWithADoubleAndUnit("/setup/shield/thickness", this);
	fShieldThickCmd->SetGuidance("Set Thickness of the shield");
	fShieldThickCmd->SetParameterName("SizeZ", false);
	fShieldThickCmd->SetRange("SizeZ>0.");
	fShieldThickCmd->SetUnitCategory("Length");
	fShieldThickCmd->AvailableForStates(G4State_Idle);
	fShieldThickCmd->SetToBeBroadcasted(false);

	fShieldPosCmd = new G4UIcmdWith3VectorAndUnit("/setup/shield/pos", this);
	fShieldPosCmd->SetGuidance("Set pos. of the shield x y z");
	fShieldPosCmd->AvailableForStates(G4State_Idle);
	fShieldPosCmd->SetToBeBroadcasted(false);

	fTestCmd = new G4UIcmdWithAString("/setup/test", this);
	fTestCmd->SetGuidance("test a function");
	fTestCmd->AvailableForStates(G4State_Idle);
	fTestCmd->SetToBeBroadcasted(false);
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	// world commands
	if (command == fWorldMaterCmd)
		fDetector->SetWorldMaterial(newValue);
	if (command == fWorldZCmd)
		fDetector->SetWorldSizeZ(fWorldZCmd->GetNewDoubleValue(newValue));
	if (command == fWorldXYCmd)
		fDetector->SetWorldSizeXY(fWorldXYCmd->GetNewDoubleValue(newValue));
	// sensor commands
	if (command == fSensorMaterCmd)
		fDetector->SetSensorMaterial(newValue);
	if (command == fSensorThickCmd)
		fDetector->SetSensorThickness(fSensorThickCmd->GetNewDoubleValue(newValue));
	if (command == fSensorSizXCmd)
		fDetector->SetSensorSizeX(fSensorSizXCmd->GetNewDoubleValue(newValue));
	if (command == fSensorSizYCmd)
		fDetector->SetSensorSizeY(fSensorSizYCmd->GetNewDoubleValue(newValue));
	if (command == fSensorPosCmd)
		fDetector->SetSensorPos(fSensorPosCmd->GetNew3VectorValue(newValue));
	if (command == fSensorRotCmd)
		fDetector->SetSensorRot(fSensorRotCmd->GetNewDoubleValue(newValue));
	// trigger commands
	if (command == fTriggerToggleCmd){
		if (fTriggerToggleCmd->GetNewIntValue(newValue) > 0)
			fDetector->SetTrigger(true);
		else
			fDetector->SetTrigger(false);
	}
	if (command == fTriggerMaterCmd)
		fDetector->SetTriggerMaterial(newValue);
	if (command == fTriggerThickCmd)
		fDetector->SetTriggerThickness(fTriggerThickCmd->GetNewDoubleValue(newValue));
	if (command == fTriggerSizXYCmd)
		fDetector->SetTriggerSizeXY(fTriggerSizXYCmd->GetNewDoubleValue(newValue));
	if (command == fTriggerPosCmd)
		fDetector->SetTriggerPos(fTriggerPosCmd->GetNew3VectorValue(newValue));
	// collimator commands
	if (command == fCollToggleCmd){
		if (fCollToggleCmd->GetNewIntValue(newValue) > 0)
			fDetector->SetCollimator(true);
		else
			fDetector->SetCollimator(false);
	}
	if (command == fCollInnerMaterCmd)
		fDetector->SetCollInnerMaterial(newValue);
	if (command == fCollOuterMaterCmd)
		fDetector->SetCollOuterMaterial(newValue);
	if (command == fCollInnerRadiusCmd)
		fDetector->SetCollInnerRadius(fCollInnerRadiusCmd->GetNewDoubleValue(newValue));
	if (command == fCollMiddleRadiusCmd)
		fDetector->SetCollMiddleRadius(fCollMiddleRadiusCmd->GetNewDoubleValue(newValue));
	if (command == fCollOuterRadiusCmd)
		fDetector->SetCollOuterRadius(fCollOuterRadiusCmd->GetNewDoubleValue(newValue));
	if (command == fCollThickCmd)
		fDetector->SetCollThickness(fCollThickCmd->GetNewDoubleValue(newValue));
	if (command == fCollPosCmd)
		fDetector->SetCollPos(fCollPosCmd->GetNew3VectorValue(newValue));
	// source shield commands
	if (command == fSourceShieldToggleCmd){
		if (fSourceShieldToggleCmd->GetNewIntValue(newValue) > 0)
			fDetector->SetSourceShield(true);
		else
			fDetector->SetSourceShield(false);
	}
	if (command == fSourceShieldMaterCmd)
		fDetector->SetSourceShieldMaterial(newValue);
	if (command == fSourceShieldThickCmd)
		fDetector->SetSourceShieldThickness(fSourceShieldThickCmd->GetNewDoubleValue(newValue));
	if (command == fSourceShieldSizXYCmd)
		fDetector->SetSourceShieldSizeXY(fSourceShieldSizXYCmd->GetNewDoubleValue(newValue));
	if (command == fSourceShieldPosCmd)
		fDetector->SetSourceShieldPos(fSourceShieldPosCmd->GetNew3VectorValue(newValue));
	// shield commands
	if (command == fShieldToggleCmd){
		if (fShieldToggleCmd->GetNewIntValue(newValue) > 0)
			fDetector->SetShield(true);
		else
			fDetector->SetShield(false);
	}
	if (command == fShieldMaterCmd)
		fDetector->SetShieldMaterial(newValue);
	if (command == fShieldThickCmd)
		fDetector->SetShieldThickness(fShieldThickCmd->GetNewDoubleValue(newValue));
	if (command == fShieldSizXYCmd)
		fDetector->SetShieldSizeXY(fShieldSizXYCmd->GetNewDoubleValue(newValue));
	if (command == fShieldPosCmd)
		fDetector->SetShieldPos(fShieldPosCmd->GetNew3VectorValue(newValue));
	// test/debug commands
	if (command == fTestCmd)
		fDetector->test();
}
