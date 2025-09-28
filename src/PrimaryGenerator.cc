#include "PrimaryGenerator.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"

PrimaryGenerator::PrimaryGenerator() {
    fParticleGun = new G4ParticleGun(1);

    // Partícula gamma
    auto particle = G4Gamma::GammaDefinition();
    fParticleGun->SetParticleDefinition(particle);

    // Energía monoenergética
    fParticleGun->SetParticleEnergy(1.25*MeV);

    // Dirección del haz
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));

    // Posición inicial
    fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,-10*cm));
}

PrimaryGenerator::~PrimaryGenerator() {
    delete fParticleGun;
}

void PrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
