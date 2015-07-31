#include "PixelDetectorSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include <iostream>

//TODO: sensor rotation is most likely not used

const G4int cMaxHits = 100; // maximum number of hits in one event that are stored

PixelDetectorSD::PixelDetectorSD(const G4String& name, const G4String& hitsCollectionName) :
		G4VSensitiveDetector(name), fHitsMap(0)
{
	collectionName.insert(hitsCollectionName);
}

PixelDetectorSD::~PixelDetectorSD()
{
}

void PixelDetectorSD::Initialize(G4HCofThisEvent* hitCollection)
{
	// Create hits collection
	fHitsMap = new DetHitsMap(SensitiveDetectorName, collectionName[0]);

	// Add this collection in hce
	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hitCollection->AddHitsCollection(hcID, fHitsMap);

	// Create hits
	for (G4int i = 0; i < cMaxHits; i++) {
		DetHit* t = new DetHit();
		fHitsMap->add(i, t);
	}
}

G4bool PixelDetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
	// energy deposit
	G4double edep = step->GetTotalEnergyDeposit();

	// step length
	G4double stepLength = 0.;
	if (step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
		stepLength = step->GetStepLength();
	}

	if (edep == 0. && stepLength == 0.)
		return false;

	G4TouchableHistory* touchable = (G4TouchableHistory*) (step->GetPreStepPoint()->GetTouchable());

	if (!touchable) {
		G4ExceptionDescription msg;
		msg << "Cannot access touchable !";
		G4Exception("PixelDetectorSD::ProcessHits()", "MyCode0004", FatalException, msg);
	}

	for (std::map<G4int, DetHit*>::iterator it = fHitsMap->GetMap()->begin(); it != fHitsMap->GetMap()->end(); ++it){
//	Per pixel steps accumulation would correspond to a simple digitization. Digitization should be independent thus no "per pixel histograming" here
		if (it->second->GetVolumeIdX() == -1){
			it->second->SetVolumeIdX(touchable->GetReplicaNumber(1));  // column
			it->second->SetVolumeIdY(touchable->GetReplicaNumber(0));  // row
			it->second->Add(edep, stepLength);
			if (step->GetTrack()->GetParticleDefinition()->GetParticleName() == "gamma"){  // gamma interaction seems to takes place as a post step process, attenuation result is checked and correct
				it->second->SetPosition(touchable->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition()));
			}
			else // not too sure what to do with electrons / protons; so I assume charge is created at the beginning of the step
				it->second->SetPosition(touchable->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition()));
			it->second->SetParticle(step->GetTrack()->GetParticleDefinition()->GetParticleName());
			break;
		}
	}

	return true;
}

void PixelDetectorSD::EndOfEvent(G4HCofThisEvent*)
{
	if (verboseLevel > 1) {
		G4int nofHits = fHitsMap->entries();
		G4cout << G4endl<< "--------> Hits Collection: in this event they are " << nofHits << " hits in the pixel detector: " << G4endl;
		for (G4int i = 0; i < nofHits; i++)
			(*fHitsMap)[i]->Print();
	}
}
