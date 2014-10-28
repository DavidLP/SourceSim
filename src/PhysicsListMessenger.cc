#include "PhysicsListMessenger.hh"

#include "PhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* pPhys)
		: G4UImessenger(),
		  fPPhysicsList(pPhys),
		  fPhysDir(0),
		  fPHadronCmd(0),
		  fPEMCmd(0),
		  fPDecayCmd(0),
		  fGammaCutCmd(0),
		  fElectCutCmd(0),
		  fProtoCutCmd(0),
		  fAllCutCmd(0),
		  fMCutCmd(0),
		  fECutCmd(0)
{
	fPhysDir = new G4UIdirectory("/setup/physics/");
	fPhysDir->SetGuidance("physics control.");

	fPHadronCmd = new G4UIcmdWithAString("/setup/physics/SelectHadronPhysics", this);
	fPHadronCmd->SetGuidance("Select hadron physics constructor.");
	fPHadronCmd->SetParameterName("PHadronPhysics", false);
	fPHadronCmd->AvailableForStates(G4State_PreInit);

	fPEMCmd = new G4UIcmdWithAString("/setup/physics/SelectEMPhysics", this);
	fPEMCmd->SetGuidance("Select electromagnetic physics constructor.");
	fPEMCmd->SetParameterName("PEMPhysics", false);
	fPEMCmd->AvailableForStates(G4State_PreInit);

	fPDecayCmd = new G4UIcmdWithAString("/setup/physics/SelectDecayPhysics", this);
	fPDecayCmd->SetGuidance("Select decay physics constructor.");
	fPDecayCmd->SetParameterName("PDecayPhysics", false);
	fPDecayCmd->AvailableForStates(G4State_PreInit);
}

PhysicsListMessenger::~PhysicsListMessenger()
{
	delete fPhysDir;
	delete fPHadronCmd;
	delete fPEMCmd;
	delete fPDecayCmd;
	delete fGammaCutCmd;
	delete fElectCutCmd;
	delete fProtoCutCmd;
	delete fAllCutCmd;
	delete fECutCmd;
	delete fMCutCmd;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
	if (command == fPHadronCmd)
		fPPhysicsList->SelectHadronPhysics(newValue);

	if (command == fPEMCmd)
		fPPhysicsList->SelectElectromagneticPhysics(newValue);

	if (command == fPDecayCmd)
		fPPhysicsList->SelectDecayPhysics(newValue);
}
