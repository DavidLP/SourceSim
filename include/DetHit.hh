#ifndef DetHit_h
#define DetHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4THitsMap.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

class DetHit: public G4VHit
{
	public:
		DetHit();
		DetHit(const DetHit&);
		virtual ~DetHit();

		// operators
		const DetHit& operator=(const DetHit&);
		const DetHit& operator+=(const DetHit&);
		G4int operator==(const DetHit&) const;

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
		void SetParticle(const G4String& particle);

		// get methods
		G4double GetEdep() const;
		G4double GetTrackLength() const;
		G4int GetVolumeIdX() const;
		G4int GetVolumeIdY() const;
		G4ThreeVector GetPosition() const;
		G4String GetParticle() const;

	private:
		G4double fEdep;        ///< Energy deposit in the sensitive volume
		G4double fTrackLength;  ///< Track length in the  sensitive volume
		G4int fVolumeIdX, fVolumeIdY;  ///< the sensitive volume identifier (here: pixel)
		G4ThreeVector fPosition; ///the position with respect to the global world mean
		G4String fParticle; ///the particle type, needed for charge cloud
};

typedef G4THitsMap<DetHit> DetHitsMap;

extern G4ThreadLocal G4Allocator<DetHit>* DetHitAllocator;

inline void* DetHit::operator new(size_t)
{
	if (!DetHitAllocator)
		DetHitAllocator = new G4Allocator<DetHit>;
	void *hit;
	hit = (void *) DetHitAllocator->MallocSingle();
	return hit;
}

inline void DetHit::operator delete(void *hit)
{
	if (!DetHitAllocator)
		DetHitAllocator = new G4Allocator<DetHit>;
	DetHitAllocator->FreeSingle((DetHit*) hit);
}

inline void DetHit::Add(G4double de, G4double dl)
{
	fEdep += de;
	fTrackLength += dl;
}

inline G4double DetHit::GetEdep() const
{
	return fEdep;
}

inline G4double DetHit::GetTrackLength() const
{
	return fTrackLength;
}

inline void DetHit::SetVolumeIdX(const G4int& volumeIdX)
{
	fVolumeIdX = volumeIdX;
}

inline void DetHit::SetVolumeIdY(const G4int& volumeIdY)
{
	fVolumeIdY = volumeIdY;
}

inline G4int DetHit::GetVolumeIdX() const
{
	return fVolumeIdX;
}

inline G4int DetHit::GetVolumeIdY() const
{
	return fVolumeIdY;
}

inline void DetHit::SetParticle(const G4String& particle)
{
	fParticle = particle;
}

inline G4String DetHit::GetParticle() const
{
	return fParticle;
}

inline void DetHit::SetPosition(const G4ThreeVector& position)
{
	fPosition = position;
}

inline G4ThreeVector DetHit::GetPosition() const
{
	return fPosition;
}

#endif
