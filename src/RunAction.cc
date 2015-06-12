#include "RunAction.hh"
#include "HistoManager.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction() :
		G4UserRunAction(), fHistoManager(0)
{
	// set printing event number per each event
	G4RunManager::GetRunManager()->SetPrintProgress(10000);

	// Book predefined histograms
	fHistoManager = new HistoManager();

	//	 Creating detector hit ntuple
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// Create directories
	analysisManager->SetHistoDirectoryName("Histograms");
	analysisManager->SetNtupleDirectoryName("EventData");

	analysisManager->SetFirstNtupleId(0);

	// Detector hit tuple (every sensor interaction creates a hit)
	analysisManager->CreateNtuple("Detector Hits", "A step within the detector volume");  // ID 0
	analysisManager->CreateNtupleIColumn(0, "Event");
	analysisManager->CreateNtupleIColumn(0, "VolumeIdX");
	analysisManager->CreateNtupleIColumn(0, "VolumeIdY");
	analysisManager->CreateNtupleDColumn(0, "EnergyDeposit");
	analysisManager->CreateNtupleDColumn(0, "TrackLength");
	analysisManager->CreateNtupleSColumn(0, "Particle");
	analysisManager->FinishNtuple(0);

	//	 Creating pixel digi hit ntuple (per pixel charge)
	analysisManager->CreateNtuple("Pixel Digits", "A pixel hit");  // ID 1
	analysisManager->CreateNtupleIColumn(1, "event");  // index
	analysisManager->CreateNtupleIColumn(1, "column");  // index
	analysisManager->CreateNtupleIColumn(1, "row");  // index
	analysisManager->CreateNtupleIColumn(1, "charge");  // [e]
	analysisManager->FinishNtuple(1);
}

RunAction::~RunAction()
{
	delete fHistoManager;
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
	//inform the runManager to save random number seed
	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	//histograms
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	if (analysisManager->IsActive()) {
		analysisManager->OpenFile();
	}
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
	// print histogram statistics
	fHistoManager->printStats();

	// save histograms
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	if (analysisManager->IsActive()) {
		analysisManager->Write();
		analysisManager->CloseFile();
	}
}

