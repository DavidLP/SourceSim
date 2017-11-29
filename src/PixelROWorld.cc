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
#include "DetectorConstruction.hh"

// Std. pixel sensor geometry, IBL planar single chip sensor
const G4int nColumns = 80;
const G4double pitchX = 250 * um;  // pixel size x width (column)
const G4int nRows = 336;
const G4double pitchY = 50 * um;  // pixel size y width (row)

PixelROWorld::PixelROWorld(G4String& parallelWorldName, DetectorConstruction* detector) :
		G4VUserParallelWorld(parallelWorldName), fSensorThickness(0), fSensorX(0), fSensorY(0), fSolidPixelSensor(0), fLogicPixelSensor(0), fPhysPixelSensor(0), fSolidPixelSensorColumn(0), fLogicPixelSensorColumn(0), fPhysPixelSensorColumn(0), fSolidPixelSensorPixel(0), fLogicPixelSensorPixel(0), fPhysPixelSensorPixel(0), fColumnSize(pitchX), fRowSize(pitchY), fPixelDetectorMessenger(0)
{
	fPixelDetectorMessenger = new PixelDetectorMessenger(this);
	// get detector info needed for pixel read out geometry
	if (detector){
		fSensorThickness = detector->GetSensorThickness();
		fSensorX = detector->GetSensorSizeX();
		fSensorY = detector->GetSensorSizeY();
	}
}

PixelROWorld::~PixelROWorld()
{
	delete fPixelDetectorMessenger;
}

void PixelROWorld::Construct()
{
	G4cout << "PixelROWorld::Construct(): Constructing pixel read out world " << G4endl;

	G4VPhysicalVolume* fPhysROworld = GetWorld();
	G4LogicalVolume* fLogicROworld = fPhysROworld->GetLogicalVolume();

	fSolidPixelSensor = new G4Box("SensorRO", fSensorX / 2, fSensorY / 2, fSensorThickness / 2.);
	fLogicPixelSensor = new G4LogicalVolume(fSolidPixelSensor, 0, "SensorROlogical");
	fPhysPixelSensor = new G4PVPlacement(0, G4ThreeVector(), fLogicPixelSensor, "SensorRO", fLogicROworld, false, 0, true);

	fSolidPixelSensorColumn = new G4Box("SensorColumnRO", pitchX / 2, fSensorY / 2, fSensorThickness / 2.);
	fLogicPixelSensorColumn = new G4LogicalVolume(fSolidPixelSensorColumn, 0, "SensorColumnROlogical", 0, 0, 0);

	fSolidPixelSensorPixel = new G4Box("SensorPixelRO", pitchX / 2, pitchY / 2, fSensorThickness / 2.);
	fLogicPixelSensorPixel = new G4LogicalVolume(fSolidPixelSensorPixel, 0, "SensorPixelROlogical", 0, 0, 0);
	
	// pixel offset in sensor tile in x (column), calculated by dividing the length by the number of pixels with a given pitch.
	// The left over active material is 'put to the side' and the pixels are centered.
	const G4double offsetX = (fSensorX - nColumns * pitchX) / 2;
	const G4double offsetY = (fSensorY - nRows * pitchY) / 2;  // pixel offset in sensor tile in y (row)
	fPhysPixelSensorColumn = new G4PVDivision("Divided along X-axis", fLogicPixelSensorColumn, fLogicPixelSensor, kXAxis, nColumns, pitchX, offsetX);
	fPhysPixelSensorPixel = new G4PVDivision("Divided along Y-axis", fLogicPixelSensorPixel, fLogicPixelSensorColumn, kYAxis, nRows, pitchY, offsetY);

	// Print construction
	EAxis axis;
	G4int nReplicas;
	G4double width, offset;
	G4bool consuming;
	fPhysPixelSensorColumn->GetReplicationData(axis, nReplicas, width, offset, consuming);
	G4cout << "Pixel detector geometry: " << G4endl;
	G4cout << "  Thickness " << G4BestUnit(fSolidPixelSensor->GetZHalfLength() * 2., "Length") << G4endl;
	G4cout << "  X " << G4BestUnit(fSolidPixelSensor->GetXHalfLength() * 2., "Length") << G4endl;
	G4cout << "  Y " << G4BestUnit(fSolidPixelSensor->GetYHalfLength() * 2., "Length") << G4endl;
	G4cout << "  Number of columns " << nReplicas << G4endl;
	G4cout << "  Column width " << G4BestUnit(width, "Length") << G4endl;
	G4cout << "  Column offset " << G4BestUnit(offset, "Length") << G4endl;

	fPhysPixelSensorPixel->GetReplicationData(axis, nReplicas, width, offset, consuming);
	G4cout << "  Number of rows " << nReplicas << G4endl;
	G4cout << "  Row width " << G4BestUnit(width, "Length") << G4endl;
	G4cout << "  Row offset " << G4BestUnit(offset, "Length") << G4endl;
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

double PixelROWorld::GetPixelPitchX()
{
	return pitchX;
}

double PixelROWorld::GetPixelPitchY()
{
	return pitchY;
}

double PixelROWorld::GetSensorThickness()
{
	return fSensorThickness;
}

int PixelROWorld::GetNcolumns()
{
	return nColumns;
}

int PixelROWorld::GetNrows()
{
	return nRows;
}
