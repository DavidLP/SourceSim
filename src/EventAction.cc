#include "EventAction.hh"
#include "HistoManager.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Digitizer.hh"
#include "G4DigiManager.hh"

#include "Randomize.hh"
#include <iomanip>

#include "G4VUserPrimaryGeneratorAction.hh"

EventAction::EventAction() :
		G4UserEventAction(), fSensorEdepHCID(-1), fSensorTrackLengthHCID(-1), fSensorTrackAngleInHCID(-1), fSensorTrackAngleOutHCID(-1), fTriggerHCID(-1), fShieldInHCID(-1), fShieldOutHCID(-1), fPixelDetectorHCID(-1)
{
	G4DigiManager::GetDMpointer()->AddNewModule(new Digitizer("PixelDigitizer"));
}

EventAction::~EventAction()
{
}

G4THitsMap<G4double>* EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
	G4THitsMap<G4double>* hitsCollection = static_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(hcID));

	if (!hitsCollection)
		G4Exception("EventAction::GetHitsCollection()", "Cannot access hits collection", FatalException, "");

	return hitsCollection;
}

DetHitsMap* EventAction::GetPixelHitsMap(G4int hcID, const G4Event* event) const
{
	DetHitsMap* hitsCollection = static_cast<DetHitsMap*>(event->GetHCofThisEvent()->GetHC(hcID));

	if (!hitsCollection)
		G4Exception("EventAction::GetHitsCollection()", "Cannot access pixel hits Collection", FatalException, "");

	return hitsCollection;
}

G4double EventAction::GetSum(G4THitsMap<G4double>* hitsMap) const
{
	G4double sumValue = 0;
	for (std::map<G4int, G4double*>::iterator it = hitsMap->GetMap()->begin(); it != hitsMap->GetMap()->end(); ++it)
		sumValue += *(it->second);

	return sumValue;
}

void EventAction::PrintEventStatistics(G4double edep, G4double trackLength) const
{
	// Print event statistics
	G4cout << "   Total energy in sensor: " << std::setw(7) << G4BestUnit(edep, "Energy") << "   Total track length in sensor: " << G4BestUnit(trackLength, "Length") << G4endl;
}

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
}

void EventAction::EndOfEventAction(const G4Event* event)
{
	// Get hit collections IDs
	if (fSensorEdepHCID == -1) {
		fSensorEdepHCID = G4SDManager::GetSDMpointer()->GetCollectionID("Detector/EnergyDeposit");
		fSensorTrackLengthHCID = G4SDManager::GetSDMpointer()->GetCollectionID("Detector/TrackLength");
		fSensorTrackAngleInHCID = G4SDManager::GetSDMpointer()->GetCollectionID("Detector/TrackAngleIn");
		fSensorTrackAngleOutHCID = G4SDManager::GetSDMpointer()->GetCollectionID("Detector/TrackAngleOut");
		fTriggerHCID = G4SDManager::GetSDMpointer()->GetCollectionID("Trigger/TriggerMechanism");
		fShieldInHCID = G4SDManager::GetSDMpointer()->GetCollectionID("SourceShield/MeasureEnergyIn");
		fShieldOutHCID = G4SDManager::GetSDMpointer()->GetCollectionID("SourceShield/MeasureEnergyOut");
		fPixelDetectorHCID = G4SDManager::GetSDMpointer()->GetCollectionID("PixelDetektor/PixelHitCollection");
	}

	// get analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	G4double edep = 0.;
	G4double trackLength = 0.;

	if (fSensorEdepHCID != -1) {
		G4THitsMap<G4double>* hcEloss = GetHitsCollection(fSensorEdepHCID, event);
		edep = GetSum(hcEloss);
		analysisManager->FillH1(7, edep);
	}

	if (fSensorTrackLengthHCID != -1) {
		G4THitsMap<G4double>* hcTLength = GetHitsCollection(fSensorTrackLengthHCID, event);
		trackLength = GetSum(hcTLength);
		analysisManager->FillH1(8, trackLength);
	}

	if (trackLength > 0. && fSensorEdepHCID != -1 && fSensorTrackLengthHCID != -1) {
		analysisManager->FillH1(12, edep / trackLength);
	}

	if (fSensorTrackAngleInHCID != -1) {
		G4THitsMap<G4double>* hcInAngle = GetHitsCollection(fSensorTrackAngleInHCID, event);
		for (std::map<G4int, G4double*>::iterator it = hcInAngle->GetMap()->begin(); it != hcInAngle->GetMap()->end(); ++it)
			analysisManager->FillH1(9, *(it->second));
	}

	if (fSensorTrackAngleOutHCID != -1) {
		G4THitsMap<G4double>* hcOutAngle = GetHitsCollection(fSensorTrackAngleOutHCID, event);
		for (std::map<G4int, G4double*>::iterator it = hcOutAngle->GetMap()->begin(); it != hcOutAngle->GetMap()->end(); ++it) {
			G4double theta = *(it->second);
			G4double dteta = analysisManager->GetH1Width(4);
			G4double unit = analysisManager->GetH1Unit(4);
			G4double weight = (unit * unit) / (CLHEP::twopi * std::sin(theta) * dteta);
			analysisManager->FillH1(10, theta, weight);
		}
	}

	if (fTriggerHCID != -1) {
		G4THitsMap<G4double>* hcTrigger = GetHitsCollection(fTriggerHCID, event);
		if (hcTrigger->GetSize() > 0)  // there is at least one hit in the trigger volume
			analysisManager->FillH1(11, edep);
	}

	if (fShieldInHCID != -1) {
		G4THitsMap<G4double>* hcInEnergy = GetHitsCollection(fShieldInHCID, event);
		for (std::map<G4int, G4double*>::iterator it = hcInEnergy->GetMap()->begin(); it != hcInEnergy->GetMap()->end(); ++it)
			analysisManager->FillH1(5, *(it->second));
	}

	if (fShieldOutHCID != -1) {
		G4THitsMap<G4double>* hcOutEnergy = GetHitsCollection(fShieldOutHCID, event);
		for (std::map<G4int, G4double*>::iterator it = hcOutEnergy->GetMap()->begin(); it != hcOutEnergy->GetMap()->end(); ++it)
			analysisManager->FillH1(6, *(it->second));
	}

	G4int eventID = event->GetEventID();

	// Fill detector hits array (not digitized data)
	if (fPixelDetectorHCID != -1) {
		DetHitsMap* pixelhitmap = GetPixelHitsMap(fPixelDetectorHCID, event);
		// fill ntuple
		for (std::map<G4int, DetHit*>::iterator it = pixelhitmap->GetMap()->begin(); it != pixelhitmap->GetMap()->end(); ++it) {
			if (it->second->GetVolumeIdX() == -1)  // speed up, do not loop empty DetHits
				break;
			analysisManager->FillNtupleIColumn(0, 0, eventID);
			analysisManager->FillNtupleIColumn(0, 1, it->second->GetVolumeIdX());
			analysisManager->FillNtupleIColumn(0, 2, it->second->GetVolumeIdY());
			analysisManager->FillNtupleDColumn(0, 3, it->second->GetEdep());
			analysisManager->FillNtupleDColumn(0, 4, it->second->GetTrackLength());
			analysisManager->AddNtupleRow(0);
//			G4cout << "   Position in sensor: " << std::setw(7) << G4BestUnit(it->second->GetPosition(), "Length") << G4endl;
		}
	}

	// Fill detector digits array (=pixel hits)
	Digitizer* pixelDigitizer = (Digitizer*) G4DigiManager::GetDMpointer()->FindDigitizerModule("PixelDigitizer");
	if (pixelDigitizer) {
		pixelDigitizer->Digitize();
		G4int fPixelDCID = G4DigiManager::GetDMpointer()->GetDigiCollectionID("PixelDigitizer/PixelDigitsCollection");
		if (fPixelDCID != -1) {
			if (G4DigiManager::GetDMpointer()->GetDigiCollection(fPixelDCID) != 0) {
				PixelDigitsCollection* digitsCollection = static_cast<PixelDigitsCollection*>(event->GetDCofThisEvent()->GetDC(fPixelDCID));
				for (G4int iDigits = 0; iDigits < digitsCollection->entries(); ++iDigits) {
					analysisManager->FillNtupleIColumn(1, 0, eventID);
					analysisManager->FillNtupleIColumn(1, 1, (*digitsCollection)[iDigits]->GetColumn());
					analysisManager->FillNtupleIColumn(1, 2, (*digitsCollection)[iDigits]->GetRow());
					analysisManager->FillNtupleIColumn(1, 3, G4int((*digitsCollection)[iDigits]->GetCharge()));  // convert charge to int value; is multiple of [e]
					analysisManager->AddNtupleRow(1);
				}
			}
			else
				G4Exception("EventAction::EndOfEventAction", "Cannot access pixel digits", FatalException, "");
		}
		else
		G4Exception("EventAction::EndOfEventAction", "Cannot access pixel digits", FatalException, "");
	}
	else
	G4Exception("EventAction::EndOfEventAction", "Cannot find digitization module.", JustWarning, "");

//print per event (modulo n)
	G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
	if ((printModulo > 0) && (eventID % printModulo == 0)) {
		G4cout << "---- End of event: " << eventID << " ----" << G4endl;
		PrintEventStatistics(edep, trackLength);
	}

//	G4cout << "------- Event ID " << event->GetEventID() << G4endl;
//	G4cout << "  ----- Primary Particle " << G4endl;
//	event->GetPrimaryVertex()->GetPrimary()-> Print();
//	G4cout << "  ----- Tracks " << event->GetTrajectoryContainer()->size() << G4endl;
//	for(size_t i = 0; i < event->GetTrajectoryContainer()->size(); ++i){
////		G4TrajectoryContainer* container = event->GetTrajectoryContainer();
//		G4VTrajectory* trajectory = (*event->GetTrajectoryContainer())[i];
//		G4cout << "------- Track "<<i<<" ID " << trajectory->GetTrackID() << G4endl;
//		G4cout << "------- Track "<<i<<" parent ID " << trajectory->GetParentID() << G4endl;
//		G4cout << "------- Track "<<i<<" particle name " << trajectory->GetParticleName() << G4endl;
//	}
			}

