#ifndef EventAction_h
#define EventAction_h 1

#include "DetHit.hh"
#include "G4UserEventAction.hh"

#include "G4THitsMap.hh"
#include "globals.hh"

class EventAction: public G4UserEventAction {
	public:
		EventAction();
		virtual ~EventAction();

		virtual void BeginOfEventAction(const G4Event* event);
		virtual void EndOfEventAction(const G4Event* event);

	private:
		// methods
		G4THitsMap<G4double>* GetHitsCollection(G4int hcID, const G4Event* event) const;
		DetHitsMap* GetPixelHitsMap(G4int hcID, const G4Event* event) const;
		G4double GetSum(G4THitsMap<G4double>* hitsMap) const;
		void PrintEventStatistics(G4double edep, G4double trackLength) const;

		// data members
		G4int fSensorEdepHCID, fSensorTrackLengthHCID, fSensorTrackAngleInHCID, fSensorTrackAngleOutHCID, fTriggerHCID, fShieldInHCID, fShieldOutHCID, fPixelDetectorHCID;
};

#endif

