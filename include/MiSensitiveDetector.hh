#ifndef MISENSITIVEDETECTOR_HH
#define MISENSITIVEDETECTOR_HH 

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHit.hh"
#include "MiHit.hh"

class MiSensitiveDetector : public G4VSensitiveDetector {
public:
  MiSensitiveDetector(const G4String& name);
  virtual ~MiSensitiveDetector();

  void Initialize(G4HCofThisEvent* hce) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

private:
  MiHitsCollection* hitsCollection;
};

#endif // MISENSITIVEDETECTOR_HH
