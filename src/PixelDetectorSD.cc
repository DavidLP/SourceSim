// Creates detector hits in a hit collection from the step information. The hit collection is reset for every event.
// The hits have nothing to do with real hits and are more like the smallest step quantity the simulation does!

#include "PixelDetectorSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <iostream>

//TODO: sensor rotation is most likely not used

const G4int cMaxHits = 1000; // maximum number of hits in one event that are stored
const G4double cMaxStepLength = 5 * um;

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

	// Store the first not filled index of the hit collection for speed up
	it_start = fHitsMap->GetMap()->begin();
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

	// Hit information from step
	G4ThreeVector preStepPosition = touchable->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition());
	G4ThreeVector postStepPosition = touchable->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition());
	G4String particle = step->GetTrack()->GetParticleDefinition()->GetParticleName();
	G4int volumeIDx = touchable->GetReplicaNumber(1); // column
	G4int volumeIDy = touchable->GetReplicaNumber(0); // row

	if (step->GetTrack()->GetParticleDefinition()->GetParticleName() == "gamma"){  // gamma interaction seems to takes place as a post step process, attenuation result is checked and correct
		if (AddHit(edep, stepLength, volumeIDx, volumeIDy, postStepPosition, particle) == false){
			G4ExceptionDescription msg;
			msg << "More than " << cMaxHits << " hits per event. Buffer too small.";
			G4Exception("PixelDetectorSD", "Cannot store all hits.", EventMustBeAborted, msg);
		}
	}
	else{
		// To increase the charge sharing simulation precision create artificial hits along along step
//		if (stepLength > cMaxStepLength * um){
//			G4int nSteps = stepLength / (cMaxStepLength * um);
//			G4ThreeVector dxyz = (postStepPosition - preStepPosition) / G4double(nSteps);
////			G4cout << G4endl<< "--------> Big step sub divided into " << nSteps << " steps " << G4endl;
////			G4cout << G4endl<< "--------> Prestep " << preStepPosition << G4endl;
////			G4cout << G4endl<< "--------> Poststep " << postStepPosition << G4endl;
////			G4cout << G4endl<< "--------> dxyz " << dxyz << G4endl;
//
//			for (auto i=0; i<nSteps; ++i){
//				G4cout << G4endl<< "----------> position " << preStepPosition + i * dxyz<< G4endl;
//				if (AddHit(edep, stepLength, volumeIDx, volumeIDy, preStepPosition + i * dxyz, particle) == false){
//					G4ExceptionDescription msg;
//					msg << "More than " << cMaxHits << " hits per event. Buffer too small.";
//					G4Exception("PixelDetectorSD", "Cannot store all hits.", EventMustBeAborted, msg);
//				}
//			}
//
//
//		}
//		else{
			if (AddHit(edep, stepLength, volumeIDx, volumeIDy, preStepPosition, particle) == false){
				G4ExceptionDescription msg;
				msg << "More than " << cMaxHits << " hits per event. Buffer too small.";
				G4Exception("PixelDetectorSD", "Cannot store all hits.", EventMustBeAborted, msg);
			}
//		}
	}

//	for (std::map<G4int, DetHit*>::iterator it = fHitsMap->GetMap()->begin(); it != fHitsMap->GetMap()->end(); ++it){
//		//	Per pixel steps accumulation would correspond to a simple digitization. Digitization should be independent thus no "per pixel histograming" here
//		//  Create new hit from each step info
//		if (it->second->GetVolumeIdX() == -1){  // Initialized to -1 thus not set
//			it->second->SetVolumeIdX(touchable->GetReplicaNumber(1));  // column
//			it->second->SetVolumeIdY(touchable->GetReplicaNumber(0));  // row
//			it->second->Add(edep, stepLength);
//			if (step->GetTrack()->GetParticleDefinition()->GetParticleName() == "gamma"){  // gamma interaction seems to takes place as a post step process, attenuation result is checked and correct
//				it->second->SetPosition(postStepPosition);
//			}
//			else{ // not too sure what to do with electrons / protons; so I assume charge is created at the beginning of the step
//				it->second->SetPosition(preStepPosition);
//				std::cout<<"\nHIT PRESTEP: "<<preStepPosition<<std::endl;
//				std::cout<<"\nHIT POSTSTEP: "<<postStepPosition<<std::endl;
//				std::cout<<"\nHIT STEPLENGTH: " << G4BestUnit(stepLength, "Length") <<std::endl;
//			}
//			it->second->SetParticle(step->GetTrack()->GetParticleDefinition()->GetParticleName());
//			break;  // break because only one hit from step can be added ?
//		}
//	}

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

bool PixelDetectorSD::AddHit(G4double& edep, G4double& dl, G4int& volumeIDx, G4int& volumeIDy, G4ThreeVector& position, G4String& particle)
{
	for (std::map<G4int, DetHit*>::iterator it = it_start; it != fHitsMap->GetMap()->end(); ++it){
		if (it->second->GetVolumeIdX() == -1){  // Initialized to -1 thus not set
			it->second->SetVolumeIdX(volumeIDx);  // column
			it->second->SetVolumeIdY(volumeIDy);  // row
			it->second->Add(edep, dl);
			it->second->SetPosition(position);
			it->second->SetParticle(particle);
			it_start = ++it;
			return true;
		}
	}
	return false;  // Cannot be added, array full
}
