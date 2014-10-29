// Simulation of different particles sources penetrating thin silicon detectors.
// University of Bonn, Silab 2014, David-Leon Pohl
// based on the GEANT4 framework

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "FTFP_BERT.hh"
#include "LBE.hh"
#include "QGSP_BERT.hh"
#include "PhysicsList.hh"

#include "RadioactiveDecayPhysics.hh"

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

namespace {
  void PrintUsage() {
    G4cerr<<" Usage: "<<G4endl;
    G4cerr<<" example [-m macro ] [-u UIsession] [-t nThreads]"<<G4endl;
    G4cerr<<"   note: -t option is available only for multi-threaded mode."<<G4endl;
  }
}

int main(int argc,char** argv)
{
  // Evaluate arguments
  if ( argc > 7 ) {
    PrintUsage();
    return 1;
  }

  G4String macro;
  G4String session;
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
#endif
    else {
      PrintUsage();
      return 1;
    }
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the MT run manager
#ifdef G4MULTITHREADED
  G4MTRunManager * runManager = new G4MTRunManager;
  if ( nThreads > 0 ) {
    runManager->SetNumberOfThreads(nThreads);
  }
#else
  G4RunManager * runManager = new G4RunManager;
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Set initialization classes
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new PhysicsList()); //or: new LBE / new FTFP_BERT / new QGSP_BERT
  runManager->SetUserInitialization(new ActionInitialization());

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  if ( macro.size() ) {  // macro mode, just execute macro without visuals and init script
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  // std. mode initialize viewer
    G4UIExecutive* ui = new G4UIExecutive(argc, argv, session);
    UImanager->ApplyCommand("/control/execute init.mac");  // run init script
    UImanager->ApplyCommand("/control/execute vis.mac");  // run vis script
    if (ui->IsGUI())
    	UImanager->ApplyCommand("/control/execute gui.mac");  // run gui setup script
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager; // user actions, physics_list and detector_description are owned and deleted by the run manager

  return 0;
}
