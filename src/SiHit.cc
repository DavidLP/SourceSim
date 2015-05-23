#include "SiHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<SiHit>* SiHitAllocator = 0;

SiHit::SiHit() :
		G4VHit(), fEdep(0.), fTrackLength(0.), fCopyNumber(0.)
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
	fCopyNumber = right.fCopyNumber;
}

const SiHit& SiHit::operator=(const SiHit& right)
{
	fEdep = right.fEdep;
	fTrackLength = right.fTrackLength;
	fCopyNumber = right.fCopyNumber;

	return *this;
}

const SiHit& SiHit::operator+=(const SiHit& right)
{
	fEdep += right.fEdep;
	fTrackLength += right.fTrackLength;
	fCopyNumber += right.fCopyNumber;

	return *this;
}

G4int SiHit::operator==(const SiHit& right) const
{
	return (this == &right) ? 1 : 0;
}

void SiHit::Print()
{
	G4cout << "Edep: " << std::setw(7) << G4BestUnit(fEdep, "Energy") << " track length: " << std::setw(7) << G4BestUnit(fTrackLength, "Length") << " volumne id "<<fCopyNumber<< G4endl;
}
