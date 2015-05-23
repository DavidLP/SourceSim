#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "g4root.hh"
class G4Run;
class HistoManager;

class RunAction: public G4UserRunAction {
public:
	RunAction();
	virtual ~RunAction();

	virtual void BeginOfRunAction(const G4Run*);
	virtual void EndOfRunAction(const G4Run*);

private:
	void book();
	HistoManager* fHistoManager;

};

#endif

