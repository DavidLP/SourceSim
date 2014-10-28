#include "HistoManager.hh"
#include "G4UnitsTable.hh"

#include "G4SystemOfUnits.hh"

HistoManager::HistoManager() :
		fFileName("SourceSimulation")
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4cout << "HistoManager: Using " << analysisManager->GetType() << G4endl;
	new G4UnitDefinition("keV/micrometer", "keV/um", "dedx", keV/micrometer);  // define new unit for dedx values
	book();
}

HistoManager::~HistoManager()
{
	delete G4AnalysisManager::Instance();
}

void HistoManager::book()
{
	// Create or get analysis manager, the choice of analysis technology is done via selection of a namespace in HistoManager.hh
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetFileName(fFileName);
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetActivation(true); // enable inactivation of histograms

	// Define histograms start values
	const G4int kMaxHisto = 13;
	const G4String id[] =
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25",
				"26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49" };

	const G4String title[] =
		{ "dummy", //0
				"Primary Energy Distribution", //1
				"Energy deposit in sensor", //2
				"Track length in sensor", //3
				"Track Out angle", //4
				"Track Out angle", //5
				"Source primary energy Distribution", //6
				"Primary x Distribution", //7
				"Primary y Distribution", //8
				"Energy deposit in sensor(triggered)", //9
				"Primary Energy Distribution Before Shield", //10
				"Primary Energy Distribution After Shield", //11
				"dE/dx in sensor" //12
			};

	dataType.push_back("NotSupported"); //0
	dataType.push_back("Energy"); //1
	dataType.push_back("Energy");//2
	dataType.push_back("Length");//3
	dataType.push_back("Angle");//4
	dataType.push_back("Angle");//5
	dataType.push_back("Energy");//6
	dataType.push_back("Length");//7
	dataType.push_back("Length");//8
	dataType.push_back("Energy");//9
	dataType.push_back("Energy");//10
	dataType.push_back("Energy");//11
	dataType.push_back("dedx");//12

	// Default values (to be reset via /analysis/h1/set command)
	G4int nbins = 100;
	G4double vmin = 0.;
	G4double vmax = 100.;

	// Create all histograms as inactivated; nbins, vmin, vmax are set via macro
	G4cout << "HistoManager: Booking histograms:\nid\tcontent" << G4endl;
	for (G4int i = 0; i < kMaxHisto; ++i) {
		G4cout << i << "\t" << title[i] << G4endl;
		G4int ih = analysisManager->CreateH1(id[i], title[i], nbins, vmin, vmax);
		analysisManager->SetH1Activation(ih, false);
	}
}

void HistoManager::printStats()
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	for (unsigned int i = 0; i < dataType.size(); ++i) {
		if (analysisManager->GetH1(i, false, true) != 0)
		{
			G4cout << i <<"\t"<<analysisManager->GetH1Title(i) <<", 1 dim, "<< analysisManager->GetH1(i)->entries()<< " entries:\n\t\t mean = " << G4BestUnit(analysisManager->GetH1(i)->mean(), dataType[i])
			<< " rms = " << G4BestUnit(analysisManager->GetH1(i)->rms(), dataType[i]) << G4endl;
		}
		else if (analysisManager->GetH2(i, false, true) != 0){
			G4cout << i <<"\t"<<analysisManager->GetH2Title(i) <<", 2 dim, "<< analysisManager->GetH2(i)->entries()<< G4endl;
		}
		else
			G4cout << "Hist with index "<< i << " does not exist." <<G4endl;
	}
}

