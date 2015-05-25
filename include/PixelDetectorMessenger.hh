#ifndef PixelDetectorMessenger_h
#define PixelDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"
#include "PixelROWorld.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

class PixelDetectorMessenger: public G4UImessenger
{
  public:
    PixelDetectorMessenger(PixelROWorld* );
   ~PixelDetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    void defineCommands();

    PixelROWorld*      fPixelDetector;
    
    G4UIdirectory*             fSetupDir;
    G4UIdirectory*             fSensorDir;

    G4UIcmdWithADoubleAndUnit* fSensorThickCmd;
    G4UIcmdWithADoubleAndUnit* fSensorSizXCmd;
    G4UIcmdWithADoubleAndUnit* fSensorSizYCmd;
    G4UIcmdWith3VectorAndUnit* fSensorPosCmd;
    G4UIcmdWithADoubleAndUnit* fSensorRotCmd;

    G4UIcmdWith3VectorAndUnit* fSensorColCmd;
    G4UIcmdWith3VectorAndUnit* fSensorRowCmd;
};

#endif

