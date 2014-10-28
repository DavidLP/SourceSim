#include "RadioactiveDecayPhysics.hh"

#include "G4RadioactiveDecay.hh"
#include "G4GenericIon.hh"
#include "globals.hh"
#include "G4PhysicsListHelper.hh"
#include "G4PhysicsConstructorFactory.hh"

G4_DECLARE_PHYSCONSTR_FACTORY(RadioactiveDecayPhysics);

RadioactiveDecayPhysics::RadioactiveDecayPhysics(G4int)
		: G4RadioactiveDecayPhysics("RadioactiveDecay")
{
}

RadioactiveDecayPhysics::RadioactiveDecayPhysics(const G4String& name)
		: G4RadioactiveDecayPhysics(name)
{
}

RadioactiveDecayPhysics::~RadioactiveDecayPhysics()
{
}

void RadioactiveDecayPhysics::ConstructProcess()
{
	G4RadioactiveDecay* rdecay = new G4RadioactiveDecay();
	rdecay->SetICM(true);
	rdecay->SetARM(true);
//	rdecay->SetDecayDirection(G4ThreeVector(0, 0, 1.));
	G4PhysicsListHelper::GetPhysicsListHelper()->RegisterProcess(rdecay, G4GenericIon::GenericIon());
}

