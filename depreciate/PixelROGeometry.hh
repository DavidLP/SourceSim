#ifndef PixelROGeometry_h
#define PixelROGeometry_h 1

#include "G4VReadOutGeometry.hh"
#include "G4VSensitiveDetector.hh"

class DetectorConstruction;

class DummySD: public G4VSensitiveDetector {
	public:
		DummySD(G4String name) :
				G4VSensitiveDetector(name)
		{
		}
		;
		~DummySD()
		{
		}
		;

		void Initialize(G4HCofThisEvent*)
		{
		}
		;
		G4bool ProcessHits(G4Step*, G4TouchableHistory*)
		{
			std::cout<<"DummySD::ProcessHits"<<std::endl;
			return false;
		}
		void EndOfEvent(G4HCofThisEvent*)
		{
		}
		;
		void clear()
		{
		}
		;
		void DrawAll()
		{
		}
		;
		void PrintAll()
		{
		}
		;
};

class PixelROGeometry: public G4VReadOutGeometry {
	public:
		PixelROGeometry();
		PixelROGeometry(G4String);
		~PixelROGeometry();

	protected:

		G4VPhysicalVolume* Build();

	private:
		DetectorConstruction* detector; //pointer to the real geometry to get dimensions
};

#endif

