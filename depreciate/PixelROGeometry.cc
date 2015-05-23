#include "../depreciate/PixelROGeometry.hh"

#include "G4RunManager.hh"
#include "DetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

PixelROGeometry::PixelROGeometry() :
		G4VReadOutGeometry()
{
}
PixelROGeometry::PixelROGeometry(G4String aString) :
		G4VReadOutGeometry(aString)
{
	G4RunManager* runManager = G4RunManager::GetRunManager();
	detector = (DetectorConstruction*) (runManager->GetUserDetectorConstruction());
}

PixelROGeometry::~PixelROGeometry()
{
}

G4VPhysicalVolume* PixelROGeometry::Build()
{
	G4cout << "Build RO geometry from world xy, z [cm] " << detector->GetWorldSizeXY() / cm << "," << detector->GetWorldSizeZ() / cm << G4endl;
	G4cout<<"Build RO geometry from sensor xy, z [cm] "<<detector->GetSensorSizeXY() / cm<<","<<detector->GetSensorThickness() / cm<<G4endl;

	G4Material* dummyMat = new G4Material(name="dummyMat", 1., 1.*g/mole, 1.*g/cm3);

	//Builds the ReadOut World
	G4Box* fSolidWorldRO = new G4Box("World", detector->GetWorldSizeXY() / 2., detector->GetWorldSizeXY() / 2., detector->GetWorldSizeZ() / 2.);// std. dimension 10 x 10 x 20 cm
	G4LogicalVolume* fLogicWorldRO = new G4LogicalVolume(fSolidWorldRO, dummyMat, "World");
	G4PVPlacement* fPhysWorldRO = new G4PVPlacement(0, G4ThreeVector(), fLogicWorldRO, "World", 0, false, 0, true);

	// Sensor
	G4Box* fSolidSensorRO = new G4Box("ROSensor", detector->GetSensorSizeXY() / 2., detector->GetSensorSizeXY() / 2., detector->GetSensorThickness() / 2.);
	G4LogicalVolume* fLogicSensorRO = new G4LogicalVolume(fSolidSensorRO, dummyMat, "ROSensor");

	new G4PVPlacement(0,//no rotation
			G4ThreeVector(),//std. position
			fLogicSensorRO,//its logical volume
			"ROSensor",//its name
			fLogicWorldRO,//its mother volume
			false,//no boolean operation
			0);//copy number

	// Assignment of the visualization attributes to the logical volume
	fLogicSensorRO->SetVisAttributes(new G4VisAttributes(G4Colour(0.,1.,1.)));

	DummySD* dummySensi = new DummySD("dummySD");// geant4 manual: "this sensitive just needs to be there, but will not be used"
	fLogicSensorRO->SetSensitiveDetector(dummySensi);

	return fPhysWorldRO;
}

