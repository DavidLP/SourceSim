#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4NeutrinoE.hh"

#include "G4Gamma.hh"

#include "HistoManager.hh"
#include "G4SystemOfUnits.hh"

StackingAction::StackingAction()
{
}

StackingAction::~StackingAction()
{
}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
{
	//workaround for HAD_RDM_011: lifetime = -1001 means very short lived and not in the table
	if (track->GetParticleDefinition()->IsGeneralIon()) {
		double lifetime = track->GetParticleDefinition()->GetIonLifeTime();

		if (fabs(lifetime - -1001.0) < 0.1) {
			static bool once = true;
			if (once)
				G4Exception("rdecaysource", "Workaround for HAD_RDM_011", JustWarning, "Will be fixed in Geant4.10.1");
			once = false;
			const_cast<G4ParticleDefinition*>(track->GetParticleDefinition())->SetPDGLifeTime(DBL_MIN);
		}
	}

	//we are not interested in neutral particles (esp. neutrinos)
	if ( (track->GetParticleDefinition() != G4Gamma::GammaDefinition()) &&  (track->GetParticleDefinition()->GetPDGCharge () == 0) )
		return fKill;

	// store secondary track infos from decayed ion
	if (track->GetParentID() < 3 && track->GetStep() == 0 && track->GetParticleDefinition()->GetParticleName() == "e-"){
//		G4cout<<"___ Track_____"<<G4endl;
//		G4cout<<"track->GetParentID() "<<track->GetParentID()<<G4endl;
//		G4cout<<"track->GetStep() "<<track->GetStep()<<G4endl;
//		G4cout<<"track->GetParticleDefinition()->GetParticleName() "<<track->GetParticleDefinition()->GetParticleName()<<G4endl;
//		G4cout<<"track->GetMomentumDirection() "<<track->GetMomentumDirection()<<G4endl;
//		G4cout<<"track->GetKineticEnergy() "<<track->GetKineticEnergy()<<G4endl;
//		G4cout<<"track->GetGlobalTime() "<<track->GetGlobalTime()<<G4endl;
		// store primary energy distribution
		G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
		analysisManager->FillH1(6, track->GetKineticEnergy());
	}

	return fUrgent;
}
