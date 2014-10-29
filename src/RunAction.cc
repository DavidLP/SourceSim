#include "RunAction.hh"
#include "HistoManager.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction() :
		G4UserRunAction(), fHistoManager(0) {
	// set printing event number per each event
	G4RunManager::GetRunManager()->SetPrintProgress(1);

//	// Book predefined histograms
	fHistoManager = new HistoManager();

//	 Creating ntuple
//	analysisManager->CreateNtuple("DeDx", "Energy deposit and track length");
//	analysisManager->CreateNtupleDColumn("EnergyDeposit");
//	analysisManager->CreateNtupleDColumn("TrackLength");
//	analysisManager->FinishNtuple();
}

RunAction::~RunAction() {
	delete fHistoManager;
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {
	//inform the runManager to save random number seed
	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	//histograms
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	if (analysisManager->IsActive()) {
		analysisManager->OpenFile();
	}
}

void RunAction::EndOfRunAction(const G4Run* /*run*/) {
	// print histogram statistics
	fHistoManager->printStats();

	// save histograms
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	if (analysisManager->IsActive()) {
		analysisManager->Write();
		analysisManager->CloseFile();
	}
}

