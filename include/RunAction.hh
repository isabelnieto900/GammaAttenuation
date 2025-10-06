/* ------- GAMMA ATTENUATION SIMULATION -------
Autor: @isabelnieto900, @PoPPop21
Fecha: Octubre 2025
-----------------------------------------------
*/
#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "DetectorConstruction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>

#ifdef USE_ROOT
class TFile;
class TTree;
class TH1F;

// Forward declarations de tipos ROOT
typedef int Int_t;
typedef float Float_t;
typedef char Char_t;
#endif

class RunAction : public G4UserRunAction
{
public:
  RunAction(DetectorConstruction *detector);
  virtual ~RunAction();

  virtual void BeginOfRunAction(const G4Run *run);
  virtual void EndOfRunAction(const G4Run *run);

  void AddTransmittedEvent();

private:
  DetectorConstruction *detector;
  G4int totalEvents;
  G4int transmittedEvents;

#ifdef USE_ROOT
  // Variables ROOT - solo datos esenciales
  TFile *rootFile;
  TTree *attenuationTree;
  TH1F *attenuationHist;

  // Estructura para datos del Tree
  struct RunData
  {
    Int_t runID;
    Char_t material[50];
    Float_t thickness;
    Int_t totalEvents;
    Int_t transmittedEvents;
    Float_t transmissionRatio;
    Float_t attenuationCoeff;
  } runData;
#endif
};

#endif // RUNACTION_HH