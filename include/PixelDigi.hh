#ifndef PixelDigi_h
#define PixelDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class PixelDigi: public G4VDigi {

	public:

		PixelDigi(G4int column = 0, G4int row = 0, G4double charge = 0.);
		~PixelDigi();
		PixelDigi(const PixelDigi&);
		const PixelDigi& operator=(const PixelDigi&);
		int operator==(const PixelDigi&) const;

		inline void* operator new(size_t);
		inline void operator delete(void*);

		void Draw();
		void Print();

	private:

		G4int fColumn;
		G4int fRow;
		G4double fCharge;

	public:

		inline void Add(G4double Charge)
		{
			fCharge += Charge;
		}

		inline void SetColumn(const G4int& column)
		{
			fColumn = column;
		}

		inline void SetRow(const G4int& row)
		{
			fRow = row;
		}

		inline void SetCharge(const G4double& Charge)
		{
			fCharge = Charge;
		}

		inline G4int GetColumn()
		{
			return fColumn;
		}

		inline G4int GetRow()
		{
			return fRow;
		}

		inline G4double GetCharge()
		{
			return fCharge;
		}

};

typedef G4TDigiCollection<PixelDigi> PixelDigitsCollection;

extern G4ThreadLocal G4Allocator<PixelDigi> *PixelDigiAllocator;

inline void* PixelDigi::operator new(size_t)
{
	if (!PixelDigiAllocator)
		PixelDigiAllocator = new G4Allocator<PixelDigi>;
	return (void*) PixelDigiAllocator->MallocSingle();
}

inline void PixelDigi::operator delete(void* aDigi)
{
	PixelDigiAllocator->FreeSingle((PixelDigi*) aDigi);
}

#endif

