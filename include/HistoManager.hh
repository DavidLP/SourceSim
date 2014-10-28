#ifndef HistoManager_h
#define HistoManager_h 1

#include <vector>
#include "globals.hh"

#include "g4root.hh"
//#include "g4xml.hh"

class HistoManager
{
  public:
    HistoManager();
   ~HistoManager();

   void printStats();

  private:
    void book();
    G4String fFileName;
    std::vector<G4String> dataType;
};

#endif

