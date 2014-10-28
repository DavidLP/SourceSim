#ifndef MeasureEnergy_h
#define MeasureEnergy_h 1

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"
#include "G4Box.hh"
#include "G4PSDirectionFlag.hh"

class MeasureEnergy : public G4VPrimitiveScorer
{
  public: // with description
      MeasureEnergy(G4String name,G4int direction, G4int depth=0);
      virtual ~MeasureEnergy();
      inline void Weighted(G4bool flg=true) { weighted = flg; }

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
      G4int  fDirection;
      G4THitsMap<G4double>* EvtEnergyMap;
      G4bool weighted;
};

#endif

