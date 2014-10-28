#ifndef Trigger_h
#define Trigger_h 1

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"
#include "G4Box.hh"
#include "G4PSDirectionFlag.hh"

class Trigger : public G4VPrimitiveScorer
{
  public: // with description
      Trigger(G4String name, G4int depth=0);
      virtual ~Trigger();

  public: 
      virtual void Initialize(G4HCofThisEvent*);
      virtual void EndOfEvent(G4HCofThisEvent*);
      virtual void clear();
      virtual void DrawAll();
      virtual void PrintAll();

  protected: // with description
        virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);

  private:
      G4int  HCID;
      G4THitsMap<G4double>* TriggerMap;
};

#endif

