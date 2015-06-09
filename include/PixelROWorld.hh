#ifndef PixelROWorld_h
#define PixelROWorld_h 1

#include "G4VUserParallelWorld.hh"
#include "G4ThreeVector.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4PVDivision;
class PixelDetectorMessenger;
class DetectorConstruction;

class PixelROWorld: public G4VUserParallelWorld {
	public:
		PixelROWorld(G4String& parallelWorldName, DetectorConstruction* detector=0);
		virtual ~PixelROWorld();

		virtual void Construct();
		virtual void ConstructSD();

		void SetSensorThickness(G4double);
		void SetSensorSizeX(G4double);
		void SetSensorSizeY(G4double);
		void SetSensorPos(G4ThreeVector);
		void SetSensorRot(G4double);

		double GetPixelPitchX();
		double GetPixelPitchY();
		double GetSensorThickness();
		int GetNcolumns();
		int GetNrows();

		void SetColumns(const G4int& Ncolumns, const G4double& size, const G4double& offset);
		void SetRows(const G4int& Nrows, const G4double& size, const G4double& offset);

	private:
		G4double fSensorThickness;  // [um]
		G4double fSensorX;  // [um]
		G4double fSensorY;  // [um]

		G4Box* fSolidPixelSensor;
		G4LogicalVolume* fLogicPixelSensor;
		G4VPhysicalVolume* fPhysPixelSensor;

		G4Box* fSolidPixelSensorColumn;
		G4LogicalVolume* fLogicPixelSensorColumn;
		G4PVDivision* fPhysPixelSensorColumn;

		G4Box* fSolidPixelSensorPixel;
		G4LogicalVolume* fLogicPixelSensorPixel;
		G4PVDivision* fPhysPixelSensorPixel;

		G4double fColumnSize, fRowSize;

		PixelDetectorMessenger* fPixelDetectorMessenger;
};

#endif
