#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction *primary)
    : G4UImessenger(), primaryAction(primary)
{
    // Crear directorio de comandos
    primaryDir = new G4UIdirectory("/gun/");
    primaryDir->SetGuidance("Comandos para configurar el generador primario");

    // Comando para cambiar energía
    energyCmd = new G4UIcmdWithADoubleAndUnit("/gun/setEnergy", this);
    energyCmd->SetGuidance("Define la energía del fotón gamma");
    energyCmd->SetGuidance("Rango típico: 1 keV a 20 MeV");
    energyCmd->SetParameterName("energy", false);
    energyCmd->SetDefaultValue(662.0);
    energyCmd->SetDefaultUnit("keV");
    energyCmd->SetUnitCategory("Energy");
    energyCmd->SetRange("energy > 0");
    energyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
    delete energyCmd;
    delete primaryDir;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
    if (command == energyCmd)
    {
        G4double energy = energyCmd->GetNewDoubleValue(newValue);
        primaryAction->SetParticleEnergy(energy);
        G4cout << "Energía configurada: " << energy / keV << " keV" << G4endl;
    }
}