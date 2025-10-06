#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"

PhysicsList::PhysicsList()
    : G4VModularPhysicsList()
{
    // Usar G4EmStandardPhysics_option4 que incluye dispersión Rayleigh
    RegisterPhysics(new G4EmStandardPhysics_option4());

    // Registrar decaimientos
    decayPhysics = new G4DecayPhysics();
    RegisterPhysics(decayPhysics);
}

PhysicsList::~PhysicsList()
{
    // NO eliminar decayPhysics manualmente
    // G4VModularPhysicsList se encarga de la limpieza automáticamente
}