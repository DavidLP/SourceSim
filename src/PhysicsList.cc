#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
#include "RadioactiveDecayPhysics.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"

#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"

#include "G4EmExtraPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4DecayPhysics.hh"

#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList():G4VModularPhysicsList(),
		  fPMessenger(0),
		  fCutForGamma(1. * um),
		  fCutForElectron(1. * um),
		  fCutForPositron(1. * um),
		  fDetectorCuts(0),
		  fTargetCuts(0)
{
	G4LossTableManager::Instance();
	defaultCutValue = 1. * um;

	fPMessenger = new PhysicsListMessenger(this);
	SetCuts();
	SetVerboseLevel(0);
}

PhysicsList::~PhysicsList()
{
	delete fPMessenger;
}

void PhysicsList::InitStdPhysics()  // usual and sufficient physics selection, has to be called
{
	SelectDecayPhysics("decay");  // one has to be initialized, why?
	SelectDecayPhysics("radioactivedecay");
	SelectElectromagneticPhysics("emstandard_opt4");
	SelectHadronPhysics("QGSP_BIC_HP");
}

void PhysicsList::SelectHadronPhysics(const G4String& name)
{
	if (verboseLevel > 0) {
		G4cout << "PhysicsList::SelectHadronPhysics: <" << name << ">" << G4endl;
	}

	if (name == "QGSP_BERT") {
		AddExtraBuilders(false);
		ReplacePhysics(new G4HadronPhysicsQGSP_BERT(verboseLevel));
	} else if (name == "QGSP_BIC") {
		AddExtraBuilders(false);
		ReplacePhysics(new G4HadronPhysicsQGSP_BIC(verboseLevel));
	} else if (name == "QGSP_BERT_HP") {
		AddExtraBuilders(true);
		ReplacePhysics(new G4HadronPhysicsQGSP_BERT_HP(verboseLevel));
	} else if (name == "QGSP_BIC_HP") {
		AddExtraBuilders(true);
		ReplacePhysics(new G4HadronPhysicsQGSP_BIC_HP(verboseLevel));
	} else {
		G4Exception("PhysicsList", "Hadron Physics not found", JustWarning, "Please select existing hadron physics.");
	}
}

void PhysicsList::SelectElectromagneticPhysics(const G4String& name)
{
	if (verboseLevel > 0) {
		G4cout << "PhysicsList::SelectElectromagneticPhysics: <" << name << ">" << G4endl;
	}

	if (name == "emlivermore")
		ReplacePhysics(new G4EmLivermorePhysics(verboseLevel));
	else if (name == "empenelope")
		ReplacePhysics(new G4EmPenelopePhysics(verboseLevel));
	else if (name == "emstandard")
		ReplacePhysics(new G4EmStandardPhysics(verboseLevel));
	else if (name == "emstandard_opt4")
		ReplacePhysics(new G4EmStandardPhysics_option4(verboseLevel));
	else
		G4Exception("PhysicsList", "Electromagnetic Physics not found", JustWarning, "Please select existing electromagnetic physics.");
}

void PhysicsList::SelectDecayPhysics(const G4String& name)
{
	if (verboseLevel > 0)
		G4cout << "PhysicsList::SelectDecayPhysics: " << name << ">" << G4endl;

	if (name == "decay")
		ReplacePhysics(new G4DecayPhysics(verboseLevel));
	else if (name == "radioactivedecay")
		ReplacePhysics(new RadioactiveDecayPhysics(verboseLevel));
	else
		G4Exception("PhysicsList", "Decay Physics not found", JustWarning, "Please select existing decay physics.");
}

//void PhysicsList::SelectRdecayDirection(const G4String& name)
//{
//	if (fRaddecayList != 0)
//		fRaddecayList
//	} else {
//		G4Exception("PhysicsList", "No radioactive decay Physics defined", JustWarning, "Cannot set decay direction.");
//	}
//}

void PhysicsList::AddExtraBuilders(G4bool flagHP)
{
	ReplacePhysics(new G4EmExtraPhysics(verboseLevel));
	if (flagHP)
		ReplacePhysics(new G4HadronElasticPhysicsHP(verboseLevel));
	else
		ReplacePhysics(new G4HadronElasticPhysics(verboseLevel));

	ReplacePhysics(new G4StoppingPhysics(verboseLevel));
	ReplacePhysics(new G4IonBinaryCascadePhysics(verboseLevel));
	ReplacePhysics(new G4NeutronTrackingCut(verboseLevel));
}

void PhysicsList::SetCuts()
{
	SetCutsWithDefault();
	SetCutValue(fCutForGamma, "gamma");
	SetCutValue(fCutForElectron, "e-");
	SetCutValue(fCutForPositron, "e+");
//	G4cout << "!!! world cuts are set" << G4endl;
//
////	if (!fTargetCuts)
////		SetTargetCut(fCutForElectron);
////	G4Region* region = (G4RegionStore::GetInstance())->GetRegion("Target");
////	region->SetProductionCuts(fTargetCuts);
////	G4cout << "Target cuts are set" << G4endl;
////
////	if (!fDetectorCuts)
////		SetDetectorCut(fCutForElectron);
////	region = (G4RegionStore::GetInstance())->GetRegion("Detector");
////	region->SetProductionCuts(fDetectorCuts);
////	G4cout << "Detector cuts are set" << G4endl;
//
//	if (verboseLevel > 0)
//		DumpCutValuesTable();
}

void PhysicsList::SetCutForGamma(G4double cut)
{
	fCutForGamma = cut;
	SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

void PhysicsList::SetCutForElectron(G4double cut)
{
	fCutForElectron = cut;
	SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

void PhysicsList::SetCutForPositron(G4double cut)
{
	fCutForPositron = cut;
	SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

void PhysicsList::SetTargetCut(G4double cut)
{
	if (!fTargetCuts)
		fTargetCuts = new G4ProductionCuts();

	fTargetCuts->SetProductionCut(cut, idxG4GammaCut);
	fTargetCuts->SetProductionCut(cut, idxG4ElectronCut);
	fTargetCuts->SetProductionCut(cut, idxG4PositronCut);

}

void PhysicsList::SetDetectorCut(G4double cut)
{
	if (!fDetectorCuts)
		fDetectorCuts = new G4ProductionCuts();

	fDetectorCuts->SetProductionCut(cut, idxG4GammaCut);
	fDetectorCuts->SetProductionCut(cut, idxG4ElectronCut);
	fDetectorCuts->SetProductionCut(cut, idxG4PositronCut);
}
