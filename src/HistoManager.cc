#include "HistoManager.hh"
#include "G4UnitsTable.hh"

#include "G4SystemOfUnits.hh"

HistoManager::HistoManager() :
		fFileName("SourceSimulation")
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4cout << "HistoManager: Using " << analysisManager->GetType() << G4endl;
	new G4UnitDefinition("keV/micrometer", "keV/um", "dedx", keV / micrometer);  // define new unit for dedx values
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
	const G4int kMaxHisto = 18;
	const G4String id[] =
		{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25",
				"26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49" };

	const G4String title[] =
		{ "Primary energy distribution", //0
				"Primary x Distribution", //1
				"Primary y Distribution", //2
				"Secondary electrons energy distribution", //3
				"Secondary photons energy distribution", //4
				"Shield particle energy distribution (entering)", //5
				"Shield particle energy distribution (leaving)", //6
				"Sensor energy deposit", //7
				"Sensor track length", //8
				"Sensor track In angle (dTheta)", //9
				"Sensor track Out angle (dTheta/dOmega)", //10
				"Sensor energy deposit (triggered)", //11
				"Sensor dE/dx", //12
				"Sensor track length (triggered)", //13
				"Sensor dE/dx (triggered)", //14
				"Sensor energy deposit (backscattered)", //15
				"Sensor energy deposit (stuck)", //16
				"Sensor energy deposit (forward)" //17
			};

	dataType.push_back("Energy"); //0
	dataType.push_back("Length"); //1
	dataType.push_back("Length"); //2
	dataType.push_back("Energy"); //3
	dataType.push_back("Energy"); //4
	dataType.push_back("Energy"); //5
	dataType.push_back("Energy"); //6
	dataType.push_back("Energy"); //7
	dataType.push_back("Length"); //8
	dataType.push_back("Angle"); //9
	dataType.push_back("Angle"); //10
	dataType.push_back("Energy"); //11
	dataType.push_back("dedx"); //12
	dataType.push_back("Length"); //13
	dataType.push_back("dedx"); //14

	dataType.push_back("Energy"); //15
	dataType.push_back("Energy"); //16
	dataType.push_back("Energy"); //17

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
		if (analysisManager->GetH1(i, false, true) != 0) {
			G4cout << i << "\t"
					<< analysisManager->GetH1Title(i) <<", 1 dim, "<< analysisManager->GetH1(i)->entries()<< " entries:\n\t\t mean = "
					<< analysisManager->GetH1(i)->mean() << " "<< analysisManager->GetH1XAxisTitle(i)<<" rms = "
					<< analysisManager->GetH1(i)->rms() << " " << analysisManager->GetH1XAxisTitle(i)<<G4endl;
		} else if (analysisManager->GetH2(i, false, true) != 0) {
			G4cout << i << "\t" << analysisManager->GetH2Title(i) << ", 2 dim, " << analysisManager->GetH2(i)->entries() << G4endl;
		}
		else
		G4cout << "Hist with index "<< i << " does not exist." <<G4endl;
	}
}

