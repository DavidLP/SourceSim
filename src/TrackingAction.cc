#include "TrackingAction.hh"

#include "G4Track.hh"
#include "G4NeutrinoE.hh"

#include "G4Gamma.hh"

#include "HistoManager.hh"
#include "G4SystemOfUnits.hh"
#include "HistoManager.hh"

#include "G4VProcess.hh"

TrackingAction::TrackingAction()
		: fXstartAbs(0), fXendAbs(0), fPrimaryCharge(0)
{
}

TrackingAction::~TrackingAction()
{
}

void TrackingAction::PreUserTrackingAction(const G4Track* /*aTrack*/)
{
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
//	if (aTrack->GetParticleDefinition()->GetPDGCharge() == 0 && aTrack->GetCreatorProcess() != 0){
//		const G4String t = aTrack->GetCreatorProcess()->GetProcessName();
//		G4cout<<"!!! GetParticleDefinition() !"<<aTrack->GetParticleDefinition()->GetParticleType()<<G4endl;
//		G4cout<<"!!! GetProcessName() !"<<t<<G4endl;
//	}

	// delete all tracks after given position to save time
	if(aTrack->GetPosition().z() < -90.*mm)
		((G4Track*)aTrack)->SetTrackStatus(fKillTrackAndSecondaries);
}
