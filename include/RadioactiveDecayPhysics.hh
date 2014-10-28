#ifndef RadioactiveDecayPhysics_h
#define RadioactiveDecayPhysics_h 1

#include "G4RadioactiveDecayPhysics.hh"

class G4RadioactiveDecay;

class RadioactiveDecayPhysics : public G4RadioactiveDecayPhysics
{
public: 
  RadioactiveDecayPhysics(G4int verbose =1);
  RadioactiveDecayPhysics(const G4String& name);
  virtual ~RadioactiveDecayPhysics();

public: 
  virtual void ConstructProcess();
};

#endif








