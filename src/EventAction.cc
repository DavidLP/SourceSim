#include "EventAction.hh"
#include "HistoManager.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include "G4VUserPrimaryGeneratorAction.hh"


EventAction::EventAction() :
		G4UserEventAction(), fSensorEdepHCID(-1), fSensorTrackLengthHCID(-1), fSensorTrackAngleInHCID(-1), fSensorTrackAngleOutHCID(-1), fTriggerHCID(-1), fShieldInHCID(-1), fShieldOutHCID(-1), fPixelDetectorHCID(-1)
{
}

EventAction::~EventAction()
{
}

G4THitsMap<G4double>* EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
	G4THitsMap<G4double>* hitsCollection = static_cast<G4THitsMap<G4double>*>(event->GetHCofThisEvent()->GetHC(hcID));

	if (!hitsCollection) {
		G4ExceptionDescription msg;
		msg << "Cannot access hits Collection ID " << hcID;
		G4Exception("EventAction::GetHitsCollection()", "MyCode0003", FatalException, msg);
	}

	return hitsCollection;
}

SiHitsMap* EventAction::GetPixelHitsMap(G4int hcID, const G4Event* event) const
{
	SiHitsMap* hitsCollection = static_cast<SiHitsMap*>(event->GetHCofThisEvent()->GetHC(hcID));

	if (!hitsCollection) {
		G4ExceptionDescription msg;
		msg << "Cannot access pixel hits Collection ID " << hcID;
		G4Exception("B4cEventAction::GetHitsCollection()", "MyCode0003", FatalException, msg);
	}

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
	// Get hist collections IDs
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
		G4cout<<"edep "<<edep<<G4endl;
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

	if (fPixelDetectorHCID != -1) {
		SiHitsMap* pixelhitmap = GetPixelHitsMap(fPixelDetectorHCID, event);
//		for (std::map<G4int, SiHit*>::iterator it = pixelhitmap->GetMap()->begin(); it != pixelhitmap->GetMap()->end(); ++it){
//			G4cout << it->first<<"   Total energy in sensor: " << std::setw(7) << G4BestUnit(it->second->GetEdep(), "Energy") << "   Total track length in sensor: " << G4BestUnit(it->second->GetTrackLength(), "Length") <<" volume id "<<it->second->GetVolumeIdentifier()<< G4endl;
//		}
		// fill ntuple
		for (std::map<G4int, SiHit*>::iterator it = pixelhitmap->GetMap()->begin(); it != pixelhitmap->GetMap()->end(); ++it){
			if (it->second->GetVolumeIdX() == -1)  // speed up
				break;
			analysisManager->FillNtupleIColumn(0, eventID);
			analysisManager->FillNtupleIColumn(1, it->second->GetVolumeIdX());
			analysisManager->FillNtupleIColumn(2, it->second->GetVolumeIdY());
			analysisManager->FillNtupleDColumn(3, it->second->GetEdep());
			analysisManager->FillNtupleDColumn(4, it->second->GetTrackLength());
			analysisManager->AddNtupleRow();
		}
	}

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

