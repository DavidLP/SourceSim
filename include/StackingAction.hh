#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

/// Stacking action class : manage the newly generated particles
///
/// One wishes do not track secondary neutrino. Therefore one kills it
/// immediately, before created particles will put in a stack.

class StackingAction : public G4UserStackingAction
{
  public:
    StackingAction();
    virtual ~StackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);        
};

#endif

