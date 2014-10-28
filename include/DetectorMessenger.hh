#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction* );
   ~DetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    void defineCommands();

    DetectorConstruction*      fDetector;
    
    G4UIdirectory*             fSetupDir;
    G4UIdirectory*             fWorldDir;
    G4UIdirectory*             fSensorDir;
    G4UIdirectory*             fTriggerDir;
    G4UIdirectory*             fCollDir;
    G4UIdirectory*             fSourceShieldDir;
    G4UIdirectory*             fShieldDir;

    G4UIcmdWithAString*        fWorldMaterCmd;
    G4UIcmdWithADoubleAndUnit* fWorldZCmd;
	G4UIcmdWithADoubleAndUnit* fWorldXYCmd;

    G4UIcmdWithAString*        fSensorMaterCmd;
    G4UIcmdWithADoubleAndUnit* fSensorThickCmd;
    G4UIcmdWithADoubleAndUnit* fSensorSizXYCmd;
    G4UIcmdWith3VectorAndUnit* fSensorPosCmd;
    G4UIcmdWithADoubleAndUnit* fSensorRotCmd;

    G4UIcmdWithAnInteger*	   fTriggerToggleCmd;
    G4UIcmdWithAString*        fTriggerMaterCmd;
	G4UIcmdWithADoubleAndUnit* fTriggerThickCmd;
	G4UIcmdWithADoubleAndUnit* fTriggerSizXYCmd;
	G4UIcmdWith3VectorAndUnit* fTriggerPosCmd;

	G4UIcmdWithAnInteger*	   fCollToggleCmd;
	G4UIcmdWithAString*        fCollInnerMaterCmd;
	G4UIcmdWithAString*        fCollOuterMaterCmd;
	G4UIcmdWithADoubleAndUnit* fCollInnerRadiusCmd;
	G4UIcmdWithADoubleAndUnit* fCollMiddleRadiusCmd;
	G4UIcmdWithADoubleAndUnit* fCollOuterRadiusCmd;
	G4UIcmdWithADoubleAndUnit* fCollThickCmd;
	G4UIcmdWith3VectorAndUnit* fCollPosCmd;

    G4UIcmdWithAnInteger*	   fSourceShieldToggleCmd;
    G4UIcmdWithAString*        fSourceShieldMaterCmd;
	G4UIcmdWithADoubleAndUnit* fSourceShieldThickCmd;
	G4UIcmdWithADoubleAndUnit* fSourceShieldSizXYCmd;
	G4UIcmdWith3VectorAndUnit* fSourceShieldPosCmd;

    G4UIcmdWithAnInteger*	   fShieldToggleCmd;
    G4UIcmdWithAString*        fShieldMaterCmd;
	G4UIcmdWithADoubleAndUnit* fShieldThickCmd;
	G4UIcmdWithADoubleAndUnit* fShieldSizXYCmd;
	G4UIcmdWith3VectorAndUnit* fShieldPosCmd;

    G4UIcmdWithAString*        fTestCmd;

};

#endif

