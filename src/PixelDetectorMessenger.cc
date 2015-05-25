#include "PixelDetectorMessenger.hh"

PixelDetectorMessenger::PixelDetectorMessenger(PixelROWorld * Det)
		: G4UImessenger(), fPixelDetector(Det),
		  fSetupDir(0),
		  fSensorThickCmd(0),
		  fSensorSizXCmd(0),
		  fSensorSizYCmd(0),
		  fSensorPosCmd(0),
		  fSensorRotCmd(0)
{
	defineCommands();
}

PixelDetectorMessenger::~PixelDetectorMessenger()
{
	delete fSensorThickCmd;
	delete fSensorSizXCmd;
	delete fSensorSizYCmd;
	delete fSensorPosCmd;
	delete fSensorRotCmd;
}

void PixelDetectorMessenger::defineCommands()
{
	fSensorDir = new G4UIdirectory("/setup/pixelsensor/");
	fSensorDir->SetGuidance("Sensor construction commands");

	fSensorSizXCmd = new G4UIcmdWithADoubleAndUnit("/setup/pixelsensor/x", this);
	fSensorSizXCmd->SetGuidance("Set sizeX of the Sensor");
	fSensorSizXCmd->SetParameterName("SizeX", false);
	fSensorSizXCmd->SetRange("SizeX>0.");
	fSensorSizXCmd->SetUnitCategory("Length");
	fSensorSizXCmd->AvailableForStates(G4State_Idle);
	fSensorSizXCmd->SetToBeBroadcasted(false);

	fSensorSizYCmd = new G4UIcmdWithADoubleAndUnit("/setup/pixelsensor/y", this);
	fSensorSizYCmd->SetGuidance("Set sizeY of the Sensor");
	fSensorSizYCmd->SetParameterName("SizeY", false);
	fSensorSizYCmd->SetRange("SizeY>0.");
	fSensorSizYCmd->SetUnitCategory("Length");
	fSensorSizYCmd->AvailableForStates(G4State_Idle);
	fSensorSizYCmd->SetToBeBroadcasted(false);

	fSensorThickCmd = new G4UIcmdWithADoubleAndUnit("/setup/pixelsensor/thickness", this);
	fSensorThickCmd->SetGuidance("Set Thickness of the Sensor");
	fSensorThickCmd->SetParameterName("SizeZ", false);
	fSensorThickCmd->SetRange("SizeZ>0.");
	fSensorThickCmd->SetUnitCategory("Length");
	fSensorThickCmd->AvailableForStates(G4State_Idle);
	fSensorThickCmd->SetToBeBroadcasted(false);

	fSensorPosCmd = new G4UIcmdWith3VectorAndUnit("/setup/pixelsensor/pos", this);
	fSensorPosCmd->SetGuidance("Set pos. of the Sensor x y z");
	fSensorPosCmd->AvailableForStates(G4State_Idle);
	fSensorPosCmd->SetToBeBroadcasted(false);

	fSensorRotCmd = new G4UIcmdWithADoubleAndUnit("/setup/pixelsensor/rot", this);
	fSensorRotCmd->SetGuidance("Set Rotation of the Sensor");
	fSensorRotCmd->SetUnitCategory("Angle");
	fSensorRotCmd->AvailableForStates(G4State_Idle);
	fSensorRotCmd->SetToBeBroadcasted(false);

	fSensorColCmd = new G4UIcmdWith3VectorAndUnit("/setup/pixelsensor/columns", this);
	fSensorColCmd->SetGuidance("Set the columns of the Sensor (number of columns, size, offset from side)");
	fSensorColCmd->AvailableForStates(G4State_Idle);
	fSensorColCmd->SetToBeBroadcasted(false);
}

void PixelDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == fSensorThickCmd)
		fPixelDetector->SetSensorThickness(fSensorThickCmd->GetNewDoubleValue(newValue));
	if (command == fSensorSizXCmd)
		fPixelDetector->SetSensorSizeX(fSensorSizXCmd->GetNewDoubleValue(newValue));
	if (command == fSensorSizYCmd)
		fPixelDetector->SetSensorSizeY(fSensorSizYCmd->GetNewDoubleValue(newValue));
	if (command == fSensorPosCmd)
		fPixelDetector->SetSensorPos(fSensorPosCmd->GetNew3VectorValue(newValue));
	if (command == fSensorRotCmd)
		fPixelDetector->SetSensorRot(fSensorRotCmd->GetNewDoubleValue(newValue));
	if (command == fSensorColCmd){
		G4ThreeVector values = fSensorPosCmd->GetNew3VectorValue(newValue);
		G4ThreeVector raw_values = fSensorPosCmd->GetNew3VectorRawValue(newValue);
		fPixelDetector->SetColumns(G4int(raw_values[0]), values[1],  values[2]);
	}
}
