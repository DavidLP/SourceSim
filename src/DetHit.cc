#include "DetHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4ThreeVector.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<DetHit>* DetHitAllocator = 0;

DetHit::DetHit() :
		G4VHit(), fEdep(0.), fTrackLength(0.), fVolumeIdX(-1), fVolumeIdY(-1), fPosition(G4ThreeVector())
{
}

DetHit::~DetHit()
{
}

DetHit::DetHit(const DetHit& right) :
		G4VHit()
{
	fEdep = right.fEdep;
	fTrackLength = right.fTrackLength;
	fVolumeIdX = right.fVolumeIdX;
	fVolumeIdY = right.fVolumeIdY;
	fPosition = right.fPosition;
	fParticle = right.fParticle;
}

const DetHit& DetHit::operator=(const DetHit& right)
{
	fEdep = right.fEdep;
	fTrackLength = right.fTrackLength;
	fVolumeIdX = right.fVolumeIdX;
	fVolumeIdY = right.fVolumeIdY;
	fPosition = right.fPosition;
	fParticle = right.fParticle;
	return *this;
}

const DetHit& DetHit::operator+=(const DetHit& right)
{
	fEdep += right.fEdep;
	fTrackLength += right.fTrackLength;
	fPosition = (fPosition + right.fPosition) / 2.;  // vector addition

	return *this;
}

G4int DetHit::operator==(const DetHit& right) const
{
	return (this == &right) ? 1 : 0;
}

void DetHit::Print()
{
	G4cout << "DetHit: Edep " << std::setw(7) << G4BestUnit(fEdep, "Energy") << ", position " << std::setw(7) << G4BestUnit(fPosition, "Length") << ", track length " << std::setw(7) << G4BestUnit(fTrackLength, "Length") << ", volumne id x/y "<<fVolumeIdX<<"/"<<fVolumeIdY<<", particle "<<fParticle<< G4endl;
}
