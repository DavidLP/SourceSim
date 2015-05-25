#include "PixelDigi.hh"

G4ThreadLocal G4Allocator<PixelDigi> *PixelDigiAllocator = 0;

PixelDigi::PixelDigi(G4int column, G4int row, G4double charge)
		: fColumn(column), fRow(row), fCharge(charge)
{
}

PixelDigi::~PixelDigi()
{
}

PixelDigi::PixelDigi(const PixelDigi& right)
		: G4VDigi()
{
	fColumn = right.fColumn;
	fRow = right.fRow;
	fCharge = right.fCharge;
}

const PixelDigi& PixelDigi::operator=(const PixelDigi& right)
{
	fColumn = right.fColumn;
	fRow = right.fRow;
	fCharge = right.fCharge;
	return *this;
}

int PixelDigi::operator==(const PixelDigi& right) const
{
	return ((fColumn == right.fColumn) && (fRow == right.fRow) && (fCharge == right.fCharge));
}

void PixelDigi::Draw()
{
}

void PixelDigi::Print()
{
}

