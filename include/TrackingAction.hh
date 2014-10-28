#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

//---------------------------------------------------------------
//
// G4UserTrackingAction.hh
//
// Description:
//   This class represents actions taken place by the user at each
//   end of stepping.
//
//---------------------------------------------------------------

class TrackingAction: public G4UserTrackingAction {
public:
	TrackingAction();
	virtual ~TrackingAction();

	virtual void PreUserTrackingAction(const G4Track* aTrack);
	virtual void PostUserTrackingAction(const G4Track* aTrack);

private:
	G4double fXstartAbs, fXendAbs;
	G4double fPrimaryCharge;
};

#endif

