#include <vector>

#include "Digitizer.hh"
#include "DigitizerMessenger.hh"
#include "PixelDigi.hh"
#include "DetHit.hh"

#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"

Digitizer::Digitizer(G4String name)
		: G4VDigitizerModule(name), fEnergyPerQ(3.74), fThreshold(0), fNoise(0), fPixelDigitsCollection(0)

{
	G4String colName = "PixelDigitsCollection";
	collectionName.push_back(colName);

	digiMessenger = new DigitizerMessenger(this);
}

Digitizer::~Digitizer()
{
	delete digiMessenger;
}

void Digitizer::Digitize()
{
	G4DigiManager* digiMan = G4DigiManager::GetDMpointer();

	// create actual event digits colection
	fPixelDigitsCollection = new PixelDigitsCollection("PixelDigitizer", "PixelDigitsCollection");

	// get actual event hits collection
	G4int pixelDetectorHCID = G4SDManager::GetSDMpointer()->GetCollectionID("PixelDetektor/PixelHitCollection");
	DetHitsMap* hitsCollection = const_cast<DetHitsMap*>(static_cast<const DetHitsMap*>(digiMan->GetHitsCollection(pixelDetectorHCID)));  // a beautiful line to show what can go wrong in C++ frameworks when too less time is spend on the API...
	if (!hitsCollection) {
		G4ExceptionDescription msg;
		msg << "Cannot access pixel hits Collection ID " << pixelDetectorHCID;
		G4Exception("Digitizer::Digitize()", "MyCode0003", FatalException, msg);
	}

	// set the total charge per digi pixel by summing all hits within the different pixel volumes (= 2D charge histograming)
	for (std::map<G4int, DetHit*>::iterator it = hitsCollection->GetMap()->begin(); it != hitsCollection->GetMap()->end(); ++it) {
		if (it->second->GetVolumeIdX() == -1)  // speed up, do not loop empty DetHits
			break;

		G4int column = it->second->GetVolumeIdX();
		G4int row = it->second->GetVolumeIdY();
		G4double charge = it->second->GetEdep() / fEnergyPerQ;

		bool digitExists = false;
		for (G4int iDigi = 0; iDigi < fPixelDigitsCollection->entries(); ++iDigi) {
			if (column == (*fPixelDigitsCollection)[iDigi]->GetColumn() && row == it->second->GetVolumeIdY()) {
				(*fPixelDigitsCollection)[iDigi]->Add(charge);
				digitExists = true;
				break;
			}
		}

		if (!digitExists) {  // pixel digi does not exist, thus create new
			PixelDigi* digi = new PixelDigi(column, row, charge);
			fPixelDigitsCollection->insert(digi);
		}
	}
}

void Digitizer::SetEnergyPerCharge(const G4double& energyPerQ)
{
	fEnergyPerQ = energyPerQ;
}

void Digitizer::SetThreshold(const G4int& threshold)
{
	fThreshold = threshold;
}

void Digitizer::SetNoise(const G4int& noise)
{
	fNoise = noise;
}

