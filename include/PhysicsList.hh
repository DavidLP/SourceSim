#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;
class G4ProductionCuts;

class PhysicsList: public G4VModularPhysicsList {
	public:
		PhysicsList();
		virtual ~PhysicsList();

		void InitStdPhysics();  // set std physics, has to be called

		// select different physics lists
		void SelectHadronPhysics(const G4String& name);
		void SelectElectromagneticPhysics(const G4String& name);
		void SelectDecayPhysics(const G4String& name);

		void SetCutForGamma(G4double);
		void SetCutForElectron(G4double);
		void SetCutForPositron(G4double);
		void SetTargetCut(G4double val);
		void SetDetectorCut(G4double val);

	private:
		void SetCuts();

		void AddExtraBuilders(G4bool flagHP);
		void DeleteHadronPhysics();

		// hide assignment operator
//		PhysicsList & operator=(const PhysicsList &right);
//		PhysicsList(const PhysicsList&);

		PhysicsListMessenger* fPMessenger;

		G4double fCutForGamma;
		G4double fCutForElectron;
		G4double fCutForPositron;
		G4ProductionCuts* fDetectorCuts;
		G4ProductionCuts* fTargetCuts;
};

#endif

