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

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
//	if (aTrack->GetTrackID() == 1) {
//		fXstartAbs = - 19.296 / 2. * um;
//		fXendAbs = 19.296 / 2. * um;
//		fPrimaryCharge = aTrack->GetDefinition()->GetPDGCharge();
//	}
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
//	if (aTrack->GetParticleDefinition()->GetPDGCharge() == 0 && aTrack->GetCreatorProcess() != 0){
//		const G4String t = aTrack->GetCreatorProcess()->GetProcessName();
//		G4cout<<"!!! GetParticleDefinition() !"<<aTrack->GetParticleDefinition()->GetParticleType()<<G4endl;
//		G4cout<<"!!! GetProcessName() !"<<t<<G4endl;
//	}
//	if(track->GetPosition().z() < -20.*mm)
//		((G4Track*)track)->SetTrackStatus(fKillTrackAndSecondaries);
//	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
//	G4ThreeVector position = aTrack->GetPosition();
//	G4ThreeVector vertex = aTrack->GetVertexPosition();
//	G4double charge = aTrack->GetDefinition()->GetPDGCharge();
//
//	G4bool transmit = ((position.z() >= fXendAbs) && (vertex.z() < fXendAbs));
//	G4bool reflect = (position.z() <= fXstartAbs);
//	G4bool notabsor = (transmit || reflect);
//
//	G4bool charged  = (charge != 0.);
//	G4ThreeVector direction = aTrack->GetMomentumDirection();
//
//	if (transmit && charged) {
//		G4double theta = std::acos(direction.z());
//		if (theta > 0.0) {
//			G4double dteta = analysisManager->GetH1Width(4);
//			G4double unit = analysisManager->GetH1Unit(4);
//			G4double weight = (unit * unit) / (CLHEP::twopi * std::sin(theta) * dteta);
//			/*
//			 G4cout << "theta, dteta, unit, weight: "
//			 << theta << "  "
//			 << dteta << "  "
//			 << unit << "  "
//			 << weight << G4endl;
//			 */
//			analysisManager->FillH1(4, theta, weight);
//		}
//	}
}
