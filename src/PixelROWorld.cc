#include "PixelROWorld.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVDivision.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#include "PixelDetectorSD.hh"

PixelROWorld::PixelROWorld(G4String& parallelWorldName) :
		G4VUserParallelWorld(parallelWorldName), fNxPixel(0), fNyPixel(0)
{

}

PixelROWorld::~PixelROWorld()
{
	;
}

void PixelROWorld::Construct()
{
	// ReadOut World
	G4VPhysicalVolume* fPhysROworld = GetWorld();
	G4LogicalVolume* fLogicROworld = fPhysROworld->GetLogicalVolume();

	G4cout<<"PixelROWorld::Construct()"<<G4endl;
	for (G4int i=0; i < fLogicROworld->GetNoDaughters(); ++i){
		G4cout<<"i "<<fLogicROworld->GetDaughter(i)->GetName()<<G4endl;

	}

	// Sensor
	G4double X = 2 * cm;
	G4double Y = 2 * cm;
	G4double pitchX = 50 * um;
	G4double pitchY = 250 * um;
	G4double thickness = 230 * um;

	G4Box* fSolidSensorRO = new G4Box("SensorRO", X / 2, Y / 2, thickness / 2.);
	G4LogicalVolume* fLogicSensorRO = new G4LogicalVolume(fSolidSensorRO, 0, "SensorROlogical");
	new G4PVPlacement(0, G4ThreeVector(), fLogicSensorRO, "SensorRO", fLogicROworld, false, 0, true);

	G4Box* fSolidSensorColumnRO = new G4Box("SensorColumnRO", pitchX / 2, Y / 2, thickness / 2.);
	G4LogicalVolume* fLogicSensorColumnRO = new G4LogicalVolume(fSolidSensorColumnRO, 0, "SensorColumnROlogical", 0, 0, 0);

	G4Box* fSolidSensorPixelRO = new G4Box("SensorPixelRO", pitchX / 2, pitchY / 2, thickness / 2.);
	G4LogicalVolume* fLogicSensorPixelRO = new G4LogicalVolume(fSolidSensorPixelRO, 0, "SensorPixelROlogical", 0, 0, 0);

	fNxPixel = G4int(X / pitchX);
	fNyPixel = G4int(Y / pitchY);
	new G4PVDivision("Divided along X-axis", fLogicSensorColumnRO, fLogicSensorRO, kXAxis, fNxPixel, 0.);
	new G4PVDivision("Divided along Y-axis", fLogicSensorPixelRO, fLogicSensorColumnRO, kYAxis, fNyPixel, 0.);
}

void PixelROWorld::ConstructSD()
{
	PixelDetectorSD * pixelDetectorSD = new PixelDetectorSD("PixelDetektor", "PixelHitCollection");
	SetSensitiveDetector("SensorPixelROlogical", pixelDetectorSD);
}
