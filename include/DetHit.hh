#ifndef SiHit_h
#define SiHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4THitsMap.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

class SiHit: public G4VHit
{
	public:
		SiHit();
		SiHit(const SiHit&);
		virtual ~SiHit();

		// operators
		const SiHit& operator=(const SiHit&);
		const SiHit& operator+=(const SiHit&);
		G4int operator==(const SiHit&) const;

		inline void* operator new(size_t);
		inline void operator delete(void*);

		// methods from base class
		virtual void Draw()
		{
		}
		virtual void Print();

		// methods to handle data
		void Add(G4double de, G4double dl);
		void SetVolumeIdX(const G4int& volumeIDx);
		void SetVolumeIdY(const G4int& volumeIDy);
		void SetPosition(const G4ThreeVector& position);

		// get methods
		G4double GetEdep() const;
		G4double GetTrackLength() const;
		G4int GetVolumeIdX() const;
		G4int GetVolumeIdY() const;
		G4ThreeVector GetPosition() const;

	private:
		G4double fEdep;        ///< Energy deposit in the sensitive volume
		G4double fTrackLength;  ///< Track length in the  sensitive volume
		G4int fVolumeIdX, fVolumeIdY;  ///< the sensitive volume identifier (here: pixel)
		G4ThreeVector fPosition; ///the position with respect to the global world mean
};

typedef G4THitsMap<SiHit> SiHitsMap;

extern G4ThreadLocal G4Allocator<SiHit>* SiHitAllocator;

inline void* SiHit::operator new(size_t)
{
	if (!SiHitAllocator)
		SiHitAllocator = new G4Allocator<SiHit>;
	void *hit;
	hit = (void *) SiHitAllocator->MallocSingle();
	return hit;
}

inline void SiHit::operator delete(void *hit)
{
	if (!SiHitAllocator)
		SiHitAllocator = new G4Allocator<SiHit>;
	SiHitAllocator->FreeSingle((SiHit*) hit);
}

inline void SiHit::Add(G4double de, G4double dl)
{
	fEdep += de;
	fTrackLength += dl;
}

inline G4double SiHit::GetEdep() const
{
	return fEdep;
}

inline G4double SiHit::GetTrackLength() const
{
	return fTrackLength;
}

inline void SiHit::SetVolumeIdX(const G4int& volumeIdX)
{
	fVolumeIdX = volumeIdX;
}

inline void SiHit::SetVolumeIdY(const G4int& volumeIdY)
{
	fVolumeIdY = volumeIdY;
}

inline G4int SiHit::GetVolumeIdX() const
{
	return fVolumeIdX;
}

inline G4int SiHit::GetVolumeIdY() const
{
	return fVolumeIdY;
}

inline void SiHit::SetPosition(const G4ThreeVector& position)
{
	fPosition = position;
}

inline G4ThreeVector SiHit::GetPosition() const
{
	return fPosition;
}

#endif
