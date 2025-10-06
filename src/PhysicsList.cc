#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"

PhysicsList::PhysicsList()
    : G4VModularPhysicsList() {
        // Registrar la física electromagnética estándar para fotones
        RegisterPhysics(new G4EmStandardPhysics());

        // Registrar decaimientos
        decayPhysics = new G4DecayPhysics();
        RegisterPhysics(decayPhysics);
    }

PhysicsList::~PhysicsList() {
    // NO eliminar decayPhysics manualmente
    // G4VModularPhysicsList se encarga de la limpieza automáticamente
}