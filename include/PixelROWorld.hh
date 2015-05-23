#ifndef PixelROWorld_h
#define PixelROWorld_h 1

#include "G4VUserParallelWorld.hh"

class PixelROWorld: public G4VUserParallelWorld
{
	public:
		PixelROWorld(G4String& parallelWorldName);
		virtual ~PixelROWorld();

	public:
		virtual void Construct();
		virtual void ConstructSD();

	private:
		G4int fNpixel;

};

#endif
