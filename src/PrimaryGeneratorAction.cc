#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    // Creamos la ´pistola de partículas
    particleGun = new G4ParticleGun(1); // 1 partícula por evento

    // Definir partícula
    G4ParticleDefinition *particle = G4Gamma::Gamma();
    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticleEnergy(662 * keV);                            // Cs-137: 662 keV (no MeV!)
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.)); // Dirección en z
    particleGun->SetParticlePosition(G4ThreeVector(0., 0., -50. * cm));   // Posición inicial
}
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete particleGun;
}
void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
    particleGun->GeneratePrimaryVertex(anEvent);
}