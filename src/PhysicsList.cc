#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList() {
    SetVerboseLevel(1);

    // Registrar los constructores individuales que componen FTFP_BERT
    RegisterPhysics(new G4EmStandardPhysics());
    RegisterPhysics(new G4EmExtraPhysics());
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4HadronElasticPhysics());
    RegisterPhysics(new G4HadronPhysicsFTFP_BERT());
    RegisterPhysics(new G4IonPhysics());
    RegisterPhysics(new G4StoppingPhysics());

    SetDefaultCutValue(0.7*mm);
}

PhysicsList::~PhysicsList() {}