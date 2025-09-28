#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name);
    ~SensitiveDetector() override {}

    void Initialize(G4HCofThisEvent*) override;
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
    void EndOfEvent(G4HCofThisEvent*) override;

private:
    G4double fEnergyDeposit;
    G4int fGammaCount;
};
