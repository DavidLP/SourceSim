#include "SiHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4ThreeVector.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<SiHit>* SiHitAllocator = 0;

SiHit::SiHit() :
		G4VHit(), fEdep(0.), fTrackLength(0.), fVolumeIdX(-1), fVolumeIdY(-1), fPosition(G4ThreeVector())
{
}

SiHit::~SiHit()
{
}

SiHit::SiHit(const SiHit& right) :
		G4VHit()
{
	fEdep = right.fEdep;
	fTrackLength = right.fTrackLength;
	fVolumeIdX = right.fVolumeIdX;
	fVolumeIdY = right.fVolumeIdY;
	fPosition = right.fPosition;
}

const SiHit& SiHit::operator=(const SiHit& right)
{
	fEdep = right.fEdep;
	fTrackLength = right.fTrackLength;
	fVolumeIdX = right.fVolumeIdX;
	fVolumeIdY = right.fVolumeIdY;
	fPosition = right.fPosition;

	return *this;
}

const SiHit& SiHit::operator+=(const SiHit& right)
{
	fEdep += right.fEdep;
	fTrackLength += right.fTrackLength;
	fPosition = (fPosition + right.fPosition) / 2.;  // vector addition

	return *this;
}

G4int SiHit::operator==(const SiHit& right) const
{
	return (this == &right) ? 1 : 0;
}

void SiHit::Print()
{
	G4cout << "Edep: " << std::setw(7) << G4BestUnit(fEdep, "Energy") << " track length: " << std::setw(7) << G4BestUnit(fTrackLength, "Length") << " volumne id x/y"<<fVolumeIdX<<"/"<<fVolumeIdY<< G4endl;
}
