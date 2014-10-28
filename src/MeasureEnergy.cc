#include "MeasureEnergy.hh"

#include "G4SystemOfUnits.hh"
#include "G4StepStatus.hh"
#include "G4Track.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"
#include "G4GeometryTolerance.hh"
#include "G4PhysicalConstants.hh"

MeasureEnergy::MeasureEnergy(G4String name, G4int direction, G4int depth) :
		G4VPrimitiveScorer(name, depth), HCID(-1), fDirection(direction), EvtEnergyMap(0), weighted(true)
{
}

MeasureEnergy::~MeasureEnergy()
{
}

void MeasureEnergy::Initialize(G4HCofThisEvent* HCE)
{
	EvtEnergyMap = new G4THitsMap<G4double>(GetMultiFunctionalDetector()->GetName(), GetName());
	if (HCID < 0)
		HCID = GetCollectionID(0);
	HCE->AddHitsCollection(HCID, (G4VHitsCollection*) EvtEnergyMap);
}

G4bool MeasureEnergy::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
	G4StepPoint* preStep = aStep->GetPreStepPoint();
	G4StepPoint* postStep = aStep->GetPostStepPoint();
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

	if (preStep->GetStepStatus() == fGeomBoundary && (fDirection == fFlux_In)) {  // Entering Geometry
		G4ThreeVector localpos_in = theTouchable->GetHistory()->GetTopTransform().TransformPoint(preStep->GetPosition());  //transform vector into sensor coordinate system
		if (std::fabs(localpos_in.z()) - boxSolid->GetZHalfLength() < kCarTolerance) {
			G4double energy_in = preStep->GetKineticEnergy();
			if (weighted)
				energy_in *= preStep->GetWeight(); // Current (Particle Weight)
			EvtEnergyMap->add(GetIndex(aStep), energy_in);
		}
	}

	if (postStep->GetStepStatus() == fGeomBoundary && (fDirection == fFlux_Out)) {  // Exiting Geometry
		G4ThreeVector localpos_out = theTouchable->GetHistory()->GetTopTransform().TransformPoint(postStep->GetPosition());  //transform vector into sensor coordinate system
		if (std::fabs(localpos_out.z()) - boxSolid->GetZHalfLength() < kCarTolerance) {
			G4double energy_out = postStep->GetKineticEnergy();
			if (weighted)
				energy_out *= preStep->GetWeight(); // Current (Particle Weight)
			EvtEnergyMap->add(GetIndex(aStep), energy_out);
		}
	}

	return TRUE;
}

void MeasureEnergy::EndOfEvent(G4HCofThisEvent*)
{
	;
}

void MeasureEnergy::clear()
{
	EvtEnergyMap->clear();
}

void MeasureEnergy::DrawAll()
{
	;
}

void MeasureEnergy::PrintAll()
{
	G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
	G4cout << " PrimitiveScorer" << GetName() <<G4endl;
	G4cout << " Number of tracks " << EvtEnergyMap->entries() << G4endl;
	for(std::map<G4int,G4double*>::iterator itr = EvtEnergyMap->GetMap()->begin(); itr != EvtEnergyMap->GetMap()->end(); itr++)
	G4cout << "  copy no.: " << itr->first << "  energy  : " << *(itr->second) << " [" << GetUnit() <<"]" << G4endl;
}

