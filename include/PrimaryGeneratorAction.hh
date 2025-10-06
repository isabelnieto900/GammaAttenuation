#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ParticleGun.hh"

class PrimaryGeneratorMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event *event);

  // Método para cambiar la energía dinámicamente
  void SetParticleEnergy(G4double energy);

private:
  G4ParticleGun *particleGun;
  PrimaryGeneratorMessenger *messenger;
};

#endif // PRIMARYGENERATORACTION_HH
