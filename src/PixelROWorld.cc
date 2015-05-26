#include "PixelROWorld.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVDivision.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "PixelDetectorSD.hh"
#include "PixelDetectorMessenger.hh"

// Std. pixel sensor geometry, IBL planar sensor
const G4double X = 2 * cm;
const G4double Y = 2 * cm;
const G4double pitchX = 50 * um;
const G4double pitchY = 250 * um;
const G4double thickness = 200 * um;

PixelROWorld::PixelROWorld(G4String& parallelWorldName)
		: G4VUserParallelWorld(parallelWorldName),
		  fSolidPixelSensor(0),
		  fLogicPixelSensor(0),
		  fPhysPixelSensor(0),
		  fSolidPixelSensorColumn(0),
		  fLogicPixelSensorColumn(0),
		  fPhysPixelSensorColumn(0),
		  fSolidPixelSensorPixel(0),
		  fLogicPixelSensorPixel(0),
		  fPhysPixelSensorPixel(0),
		  fColumnSize(pitchX),
		  fRowSize(pitchY),
		  fPixelDetectorMessenger(0)
{
	fPixelDetectorMessenger = new PixelDetectorMessenger(this);
}

PixelROWorld::~PixelROWorld()
{
	delete fPixelDetectorMessenger;
}

void PixelROWorld::Construct()
{
	G4VPhysicalVolume* fPhysROworld = GetWorld();
	G4LogicalVolume* fLogicROworld = fPhysROworld->GetLogicalVolume();

	fSolidPixelSensor = new G4Box("SensorRO", X / 2, Y / 2, thickness / 2.);
	fLogicPixelSensor = new G4LogicalVolume(fSolidPixelSensor, 0, "SensorROlogical");
	fPhysPixelSensor = new G4PVPlacement(0, G4ThreeVector(), fLogicPixelSensor, "SensorRO", fLogicROworld, false, 0, true);

	fSolidPixelSensorColumn = new G4Box("SensorColumnRO", pitchX / 2, Y / 2, thickness / 2.);
	fLogicPixelSensorColumn = new G4LogicalVolume(fSolidPixelSensorColumn, 0, "SensorColumnROlogical", 0, 0, 0);

	fSolidPixelSensorPixel = new G4Box("SensorPixelRO", pitchX / 2, pitchY / 2, thickness / 2.);
	fLogicPixelSensorPixel = new G4LogicalVolume(fSolidPixelSensorPixel, 0, "SensorPixelROlogical", 0, 0, 0);

	fPhysPixelSensorColumn = new G4PVDivision("Divided along X-axis", fLogicPixelSensorColumn, fLogicPixelSensor, kXAxis, G4int(X / fColumnSize), 0.);
	fPhysPixelSensorPixel = new G4PVDivision("Divided along Y-axis", fLogicPixelSensorPixel, fLogicPixelSensorColumn, kYAxis, G4int(Y / fRowSize), 0.);
}

void PixelROWorld::SetColumns(const G4int& Ncolumns, const G4double& size, const G4double& offset)
{
	fColumnSize = size;
	fSolidPixelSensorColumn->SetXHalfLength(fColumnSize / 2.);
	fLogicPixelSensor->RemoveDaughter(fPhysPixelSensorColumn);
	fPhysPixelSensorColumn->SetMotherLogical(0);
	fPhysPixelSensorColumn = new G4PVDivision("Divided along X-axis", fLogicPixelSensorColumn, fLogicPixelSensor, kXAxis, Ncolumns, offset);  // looks bad, but geant4 manual: "The registries will automatically delete those objects when requested; users should not deleted geometry objects manually"
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
//	G4cout << "Sensor columns have been set: " << Ncolumns << " columns, " << G4BestUnit(size, "Length") << " each and " << G4BestUnit(size, "Length") << " offset" << G4endl;
}

void PixelROWorld::SetRows(const G4int& Nrows, const G4double& size, const G4double& offset)
{
	fSolidPixelSensorPixel->SetXHalfLength(fColumnSize / 2.);
	fSolidPixelSensorPixel->SetYHalfLength(size / 2.);
	fLogicPixelSensorColumn->RemoveDaughter(fPhysPixelSensorPixel);
	fPhysPixelSensorPixel->SetMotherLogical(0);
	fPhysPixelSensorPixel = new G4PVDivision("Divided along Y-axis", fLogicPixelSensorPixel, fLogicPixelSensorColumn, kYAxis, Nrows, offset);  // looks bad, but geant4 manual: "The registries will automatically delete those objects when requested; users should not deleted geometry objects manually"
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor rows have been set: " << Nrows << " columns, " << G4BestUnit(size, "Length") << " each and " << G4BestUnit(size, "Length") << " offset" << G4endl;
}

void PixelROWorld::ConstructSD()
{
	PixelDetectorSD * pixelDetectorSD = new PixelDetectorSD("PixelDetektor", "PixelHitCollection");
	SetSensitiveDetector("SensorPixelROlogical", pixelDetectorSD);
}

void PixelROWorld::SetSensorPos(G4ThreeVector pos)
{
	fPhysPixelSensor->SetTranslation(pos);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor position set to " << pos << G4endl;
}

void PixelROWorld::SetSensorRot(G4double phi)
{
	// u, v, w are the daughter axes, projected on the mother frame
	G4ThreeVector u = G4ThreeVector(1, 0, 0);
	G4ThreeVector v = G4ThreeVector(0., std::cos(phi), -std::sin(phi));
	G4ThreeVector w = G4ThreeVector(0, std::sin(phi), std::cos(phi));
	G4RotationMatrix* rotm1 = new G4RotationMatrix(u, v, w);
	fPhysPixelSensor->SetRotation(rotm1);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor rotated to phi = " << phi << G4endl;
}

void PixelROWorld::SetSensorThickness(G4double val)
{
	fSolidPixelSensor->SetZHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor thickness set to " << G4BestUnit(val, "Length") << G4endl;
}

void PixelROWorld::SetSensorSizeX(G4double val)
{
	fSolidPixelSensor->SetXHalfLength(val / 2.);
	fSolidPixelSensor->SetYHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor size x set to " << G4BestUnit(val, "Length") << G4endl;
}

void PixelROWorld::SetSensorSizeY(G4double val)
{
	fSolidPixelSensor->SetXHalfLength(val / 2.);
	fSolidPixelSensor->SetYHalfLength(val / 2.);
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
	G4cout << "Sensor size y set to " << G4BestUnit(val, "Length") << G4endl;
}
