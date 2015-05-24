// Implements a pixel detector

#ifndef PixelDetectorSD_h
#define PixelDetectorSD_h 1

#include "G4VSensitiveDetector.hh"
#include <vector>
#include "DetHit.hh"

class G4Step;
class G4HCofThisEvent;

class PixelDetectorSD: public G4VSensitiveDetector
{
	public:
		PixelDetectorSD(const G4String& name, const G4String& hitsCollectionName);
		virtual ~PixelDetectorSD();

		// methods from base class
		virtual void Initialize(G4HCofThisEvent* hitCollection);
		virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
		virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

	private:
		DetHitsMap* fHitsMap;
};

#endif

