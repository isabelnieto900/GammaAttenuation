#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction *detector)
    : G4UImessenger(), detectorConstruction(detector)
{

    // Crear directorio de comandos
    detectorDir = new G4UIdirectory("/detector/");
    detectorDir->SetGuidance("Comandos para configurar el detector");

    // Comando para cambiar material
    materialCmd = new G4UIcmdWithAString("/detector/setMaterial", this);
    materialCmd->SetGuidance("Selecciona el material del absorbedor (blindaje)");
    materialCmd->SetGuidance("Materiales predefinidos: polyethylene, concrete, lead");
    materialCmd->SetGuidance("También acepta nombres directos de materiales G4 (ej: G4_POLYETHYLENE)");
    materialCmd->SetParameterName("material", false);
    materialCmd->SetDefaultValue("polyethylene");
    // Removemos SetCandidates para permitir cualquier material
    materialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Comando para cambiar espesor
    thicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/setThickness", this);
    thicknessCmd->SetGuidance("Define el espesor del material absorbedor");
    thicknessCmd->SetGuidance("Rango recomendado: 0.1 cm a 50 cm");
    thicknessCmd->SetParameterName("thickness", false);
    thicknessCmd->SetDefaultValue(5.0);
    thicknessCmd->SetDefaultUnit("cm");
    thicknessCmd->SetUnitCategory("Length");
    thicknessCmd->SetRange("thickness > 0");
    thicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
    delete materialCmd;
    delete thicknessCmd;
    delete detectorDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{

    if (command == materialCmd)
    {
        // Aceptar tanto nombres predefinidos como nombres G4 directos
        detectorConstruction->SetMaterialType(newValue);
        G4cout << "Material configurado: " << newValue << G4endl;
    }

    else if (command == thicknessCmd)
    {
        G4double thickness = thicknessCmd->GetNewDoubleValue(newValue);

        // Validación adicional de rango
        if (thickness > 0.05 * cm && thickness < 100 * cm)
        {
            detectorConstruction->SetThickness(thickness);
            G4cout << "Espesor cambiado a: " << thickness / cm << " cm" << G4endl;
        }
        else
        {
            G4cerr << "Advertencia: Espesor fuera del rango recomendado (0.05 - 100 cm)" << G4endl;
            detectorConstruction->SetThickness(thickness);
        }
    }
}