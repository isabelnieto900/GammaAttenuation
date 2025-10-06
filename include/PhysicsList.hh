#ifndef PHYSICSLIST_HH
#define PHYSICSLIST_HH 

#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4DecayPhysics.hh"

class PhysicsList : public G4VModularPhysicsList {
  public:
    PhysicsList();
    ~PhysicsList() override;

  private:
    G4DecayPhysics* decayPhysics; // Solo mantener la referencia, no eliminar manualmente
};

#endif // PHYSICSLIST_HH