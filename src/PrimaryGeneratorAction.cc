#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4UnitsTable.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction() :
		G4VUserPrimaryGeneratorAction(), fParticleSource(0)
{
	G4int nofParticles = 1;
	fParticleSource = new G4GeneralParticleSource();
	fParticleSource->SetNumberOfParticles(nofParticles);
	G4ParticleDefinition* particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("e-");
	fParticleSource->SetParticleDefinition(particleDefinition);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete fParticleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	// This function is called at the begining of event
	fParticleSource->GeneratePrimaryVertex(anEvent);

	// store primary energy distribution and position
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillH1(0, fParticleSource->GetParticleEnergy());
	G4ThreeVector position = fParticleSource->GetParticlePosition();
	analysisManager->FillH1(1, position[0]);  // x-pos hist
	analysisManager->FillH1(2, position[1]);  // y-pos hist
}

