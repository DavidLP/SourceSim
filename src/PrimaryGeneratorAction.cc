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

PrimaryGeneratorAction::PrimaryGeneratorAction() :
		G4VUserPrimaryGeneratorAction(), fParticleSource(0) {
	G4int nofParticles = 1;
	fParticleSource = new G4GeneralParticleSource();
	fParticleSource->SetNumberOfParticles(nofParticles);
	G4ParticleDefinition* particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("e-");
	fParticleSource->SetParticleDefinition(particleDefinition);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete fParticleSource;
	fParticleSource = 0;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// This function is called at the begining of event

	// In order to avoid dependence of PrimaryGeneratorAction
	// on DetectorConstruction class we get world volume
	// from G4LogicalVolumeStore
	G4double worldZHalfLength = 0;
	G4LogicalVolume* worlLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");
	G4Box* worldBox = 0;
	if (worlLV)
		worldBox = dynamic_cast<G4Box*>(worlLV->GetSolid());
	if (worldBox) {
		worldZHalfLength = worldBox->GetZHalfLength();
	} else {
		G4ExceptionDescription msg;
		msg << "World volume of box not found." << G4endl;
		msg << "Perhaps you have changed geometry." << G4endl;
		msg << "The gun will be place in the center.";
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "MyCode0002",
				JustWarning, msg);
	}

	// Set source position
	fParticleSource->SetParticlePosition(G4ThreeVector(0., 0., -worldZHalfLength));
	fParticleSource->GeneratePrimaryVertex(anEvent);

// store primary energy distribution
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillH1(1, fParticleSource->GetParticleEnergy());
	G4ThreeVector position = fParticleSource->GetParticlePosition();
	analysisManager->FillH1(7, position[0]);
	analysisManager->FillH1(8, position[1]);
}

