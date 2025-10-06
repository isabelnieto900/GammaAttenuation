#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
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
    particleGun->SetParticleEnergy(662 * keV);                            // Cs-137: 662 keV (valor por defecto)
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.)); // Dirección en z
    particleGun->SetParticlePosition(G4ThreeVector(0., 0., -50. * cm));   // Posición inicial

    // Crear messenger para comandos dinámicos
    messenger = new PrimaryGeneratorMessenger(this);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete messenger;
    delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
    particleGun->GeneratePrimaryVertex(anEvent);
}

void PrimaryGeneratorAction::SetParticleEnergy(G4double energy)
{
    particleGun->SetParticleEnergy(energy);
}