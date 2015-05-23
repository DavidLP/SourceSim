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
		G4VUserParallelWorld(parallelWorldName), fNpixel(0)
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

	// Sensor
	G4double X = 2 * cm;
	G4double Y = 2 * cm;
	G4double pitchX = 50 * um;
	G4double pitchY = 50 * um;
	G4double thickness = 230 * um;

	G4Box* fSolidSensorRO = new G4Box("SensorRO", X / 2, Y / 2, thickness / 2.);
	G4LogicalVolume* fLogicSensorRO = new G4LogicalVolume(fSolidSensorRO, 0, "SensorROlogical");
	new G4PVPlacement(0, G4ThreeVector(), fLogicSensorRO, "SensorRO", fLogicROworld, false, 0, true);

	G4Box* fSolidSensorPixelRO = new G4Box("SensorPixelRO", pitchX / 2, pitchY / 2, thickness / 2.);
	G4LogicalVolume* fLogicSensorPixelRO = new G4LogicalVolume(fSolidSensorPixelRO, 0, "SensorPixelROlogical", 0, 0, 0);

	fNpixel = G4int(X / pitchX);
	new G4PVDivision("Divided along X-axis", fLogicSensorPixelRO, fLogicSensorRO, kXAxis, fNpixel, 0.);
}

void PixelROWorld::ConstructSD()
{
	PixelDetectorSD * pixelDetectorSD = new PixelDetectorSD("PixelDetektor", "PixelHitCollection", fNpixel);
	SetSensitiveDetector("SensorPixelROlogical", pixelDetectorSD);
}
