#include <vector>

#include "Digitizer.hh"
#include "DigitizerMessenger.hh"
#include "PixelDigi.hh"
#include "DetHit.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
#include "G4ios.hh"

#include "Randomize.hh"


Digitizer::Digitizer(G4String name) :
		G4VDigitizerModule(name),
		fEnergyPerCharge(3.74),
		fThreshold(2000),
		fNoise(130),
		fPixelDigitsCollection(0)

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

	// Create a temporary actual event digits collection, to be able to remove hits below threshold, G4TDigiCollection does not support deleting entries ?!
	PixelDigitsCollection* actualPixelDigitsCollection = new PixelDigitsCollection("PixelDigitizer", "PixelDigitsCollection");

	// Get actual event hits collection
	G4int pixelDetectorHCID = G4SDManager::GetSDMpointer()->GetCollectionID("PixelDetektor/PixelHitCollection");
	DetHitsMap* hitsCollection = const_cast<DetHitsMap*>(static_cast<const DetHitsMap*>(digiMan->GetHitsCollection(pixelDetectorHCID)));  // a beautiful line to show what can go wrong in C++ frameworks when too less time is spend on the API...
	if (!hitsCollection)
		G4Exception("Digitizer::Digitize()", "Cannot access pixel hits collection", FatalException, "");

	// Set the total charge per digi pixel by summing all hits within the different pixel volumes (= 2D charge histograming)
	// This is a loop with an inner loop; speed wise not the best solution, but the geant4 examples are even worse and loop all hits + digits (1 pixel = 1 digit...), here the least amount of hits / digits were created and are looped
	for (std::map<G4int, DetHit*>::iterator it = hitsCollection->GetMap()->begin(); it != hitsCollection->GetMap()->end(); ++it) {
		if (it->second->GetVolumeIdX() == -1)  // speed up, do not loop remaining and all empty DetHits
			break;

		G4int column = it->second->GetVolumeIdX();
		G4int row = it->second->GetVolumeIdY();
		G4double charge = it->second->GetEdep() / fEnergyPerCharge / eV;  // charge in electrons

		bool digitExists = false;
		for (G4int iDigi = 0; iDigi < actualPixelDigitsCollection->entries(); ++iDigi) {  // go through all already created digis
			if (column == (*actualPixelDigitsCollection)[iDigi]->GetColumn() && row == (*actualPixelDigitsCollection)[iDigi]->GetRow()) {
				(*actualPixelDigitsCollection)[iDigi]->Add(charge);
				digitExists = true;
				break;
			}
		}

		if (!digitExists) {  // pixel digi does not exist, thus create new
			PixelDigi* digi = new PixelDigi(column, row, charge);
			actualPixelDigitsCollection->insert(digi);
		}
	}

	// Create the result actual event digits colection
	fPixelDigitsCollection = new PixelDigitsCollection("PixelDigitizer", "PixelDigitsCollection");

	// Apply noise and threshold to all actual pixel digis
	for (G4int iDigi = 0; iDigi < actualPixelDigitsCollection->entries(); ++iDigi) {
		(*actualPixelDigitsCollection)[iDigi]->SetCharge(G4RandGauss::shoot((*actualPixelDigitsCollection)[iDigi]->GetCharge(), fNoise));  // add gaussian noise to the charge
		if ((*actualPixelDigitsCollection)[iDigi]->GetCharge() >= fThreshold)
			fPixelDigitsCollection->insert(new PixelDigi(*(*actualPixelDigitsCollection)[iDigi]));
	}

	StoreDigiCollection(fPixelDigitsCollection);
}

void Digitizer::SetEnergyPerCharge(const G4double& energyPerCharge)
{
	fEnergyPerCharge = energyPerCharge;
}

void Digitizer::SetThreshold(const G4int& threshold)
{
	fThreshold = threshold;
}

void Digitizer::SetNoise(const G4int& noise)
{
	fNoise = noise;
}

