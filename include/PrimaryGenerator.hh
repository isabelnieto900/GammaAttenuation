#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

class PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGenerator();
    ~PrimaryGenerator() override;
    void GeneratePrimaries(G4Event*) override;

private:
    G4ParticleGun* fParticleGun;
};
