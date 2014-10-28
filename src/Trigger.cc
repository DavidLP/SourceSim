#include "Trigger.hh"

#include "G4SystemOfUnits.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "G4GeometryTolerance.hh"
#include "G4PhysicalConstants.hh"

Trigger::Trigger(G4String name, G4int depth) :
		G4VPrimitiveScorer(name, depth), HCID(-1), TriggerMap(0)
{
	;
}

Trigger::~Trigger() {
	;
}

void Trigger::Initialize(G4HCofThisEvent* HCE) {
	TriggerMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), GetName());
	if (HCID < 0)
		HCID = GetCollectionID(0);
	HCE->AddHitsCollection(HCID, (G4VHitsCollection*) TriggerMap);
}

G4bool Trigger::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
	G4StepPoint* preStep = aStep->GetPreStepPoint();
	G4VPhysicalVolume* physVol = preStep->GetPhysicalVolume();
	G4VPVParameterisation* physParam = physVol->GetParameterisation();
	G4VSolid* solid = 0;

	if (physParam) { // for parameterized volume
		G4int idx = ((G4TouchableHistory*) (preStep->GetTouchable()))->GetReplicaNumber(indexDepth);
		solid = physParam->ComputeSolid(idx, physVol);
		solid->ComputeDimensions(physParam, idx, physVol);
	} else { // for ordinary volume
		solid = physVol->GetLogicalVolume()->GetSolid();
	}

	G4Box* boxSolid = (G4Box*) (solid);

	G4TouchableHandle theTouchable = preStep->GetTouchableHandle();
	G4double kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();

	if (preStep->GetStepStatus() == fGeomBoundary) {  // Entering Geometry
		G4ThreeVector localpos_in = theTouchable->GetHistory()->GetTopTransform().TransformPoint(preStep->GetPosition());  //transform vector into sensor coordinate system
		if(std::fabs( localpos_in.z() ) - boxSolid->GetZHalfLength() < kCarTolerance ) {
			G4double hit = 1.;
			TriggerMap->add(GetIndex(aStep), hit);
			aStep->GetTrack()->SetTrackStatus(fStopAndKill);
		}
	}

	return TRUE;
}

void Trigger::EndOfEvent(G4HCofThisEvent*)
{
	;
}

void Trigger::clear() {
	TriggerMap->clear();
}

void Trigger::DrawAll()
{
	;
}

void Trigger::PrintAll()
{
	G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
	G4cout << " PrimitiveScorer" << GetName() <<G4endl;
	G4cout << " Number of tracks " << TriggerMap->entries() << G4endl;
}

