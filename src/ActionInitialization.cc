#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "StackingAction.hh"
#include "TrackingAction.hh"

ActionInitialization::ActionInitialization() :
		G4VUserActionInitialization()
{
}

ActionInitialization::~ActionInitialization()
{
}

void ActionInitialization::BuildForMaster() const
{
	SetUserAction(new RunAction);
}

void ActionInitialization::Build() const
{
	SetUserAction(new RunAction);
	SetUserAction(new EventAction);
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new StackingAction);
	SetUserAction(new TrackingAction);
}
