/* ------- GAMMA ATTENUATION SIMULATION -------
Autor: @isabelnieto900, @PoPPop21
Fecha: Octubre 2025
-----------------------------------------------
*/
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
// Construcción del sentiveDetector para el volumen
#include "G4SDManager.hh"
#include "MiSensitiveDetector.hh"

/* Defino los valores por defecto que tenrá mi detector cuando arranque la simualción*/
DetectorConstruction::DetectorConstruction()
    : materialType("water"), thickness(5.0 * cm)
{
    messenger = new DetectorMessenger(this);
} // Material inicial es agua, con espesor de 5cm.

DetectorConstruction::~DetectorConstruction()
{
    delete messenger;
}

/* Cambiamos el material dinámicamente */
void DetectorConstruction::SetMaterialType(const G4String &material)
{ // Setmaterial -> deja elegir otro amterial desde la interfaz macros.
    materialType = material;
    // Forzar reconstrucción si ya está construido
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

/* Cambiar espesor dinámicamente */
void DetectorConstruction::SetThickness(G4double thick)
{ // Puede cambiar de 5cm a 10cm desde la interfaz macros.
    thickness = thick;
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

/* Deefinición de materiales */
G4Material *DetectorConstruction::DefineMaterials()
{
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *material = nullptr;

    // Normalizar materialType por si acaso
    G4String m = materialType;
    // opcional: forzar minúsculas si quieres (requiere transformación manual)

    if (m == "water")
    {
        material = nist->FindOrBuildMaterial("G4_WATER");
    }
    else if (m == "muscle")
    {
        // Segun ICRU Report 44
        G4double density = 1.05 * g / cm3;
        material = new G4Material("muscle", density, 9);
        material->AddElement(nist->FindOrBuildElement("H"), 10.2 * perCent);
        material->AddElement(nist->FindOrBuildElement("C"), 14.3 * perCent);
        material->AddElement(nist->FindOrBuildElement("N"), 3.4 * perCent);
        material->AddElement(nist->FindOrBuildElement("O"), 71.0 * perCent);
        material->AddElement(nist->FindOrBuildElement("Na"), 0.1 * perCent);
        material->AddElement(nist->FindOrBuildElement("P"), 0.2 * perCent);
        material->AddElement(nist->FindOrBuildElement("S"), 0.5 * perCent);
        material->AddElement(nist->FindOrBuildElement("Cl"), 0.1 * perCent);
        material->AddElement(nist->FindOrBuildElement("K"), 0.2 * perCent);
    }
    else if (m == "bone")
    {
        // Hueso segun ICRU report 44
        G4double density = 1.92 * g / cm3;
        material = new G4Material("bone", density, 9);
        material->AddElement(nist->FindOrBuildElement("H"), 3.4 * perCent);
        material->AddElement(nist->FindOrBuildElement("C"), 15.5 * perCent);
        material->AddElement(nist->FindOrBuildElement("N"), 4.2 * perCent);
        material->AddElement(nist->FindOrBuildElement("O"), 43.5 * perCent);
        material->AddElement(nist->FindOrBuildElement("Na"), 0.1 * perCent);
        material->AddElement(nist->FindOrBuildElement("Mg"), 0.2 * perCent);
        material->AddElement(nist->FindOrBuildElement("P"), 16.9 * perCent);
        material->AddElement(nist->FindOrBuildElement("S"), 0.2 * perCent);
        material->AddElement(nist->FindOrBuildElement("Ca"), 16.0 * perCent);
    }
    else if (m == "lead")
    {
        material = nist->FindOrBuildMaterial("G4_Pb");
    }
    else if (m == "concrete")
    {
        material = nist->FindOrBuildMaterial("G4_CONCRETE");
    }
    else
    {
        // Intentar buscar directamente el material por su nombre G4
        material = nist->FindOrBuildMaterial(m);
        if (!material)
        {
            G4cerr << "Material " << m << " no reconocido. Usando agua por defecto." << G4endl;
            material = nist->FindOrBuildMaterial("G4_WATER");
        }
    }

    // DIAGNÓSTICO: informar qué material se obtuvo y fallback si es null
    if (!material)
    {
        G4cerr << "ERROR: FindOrBuildMaterial devolvió NULL para: " << m << G4endl;
        G4cerr << " -> Haciendo fallback a G4_WATER para evitar crash." << G4endl;
        material = nist->FindOrBuildMaterial("G4_WATER");
        if (!material)
        {
            G4cerr << "ERROR CRÍTICO: no se pudo cargar G4_WATER. Revisa instalación de Geant4." << G4endl;
        }
    }
    else
    {
        G4cout << "Material cargado: " << material->GetName() << " (para request: " << m << ")" << G4endl;
    }

    return material;
}

/*DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}*/

/* Creación de la geometría del mundo y el detector */

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    // Mundo mínimo de aire para que compile
    G4NistManager *nist = G4NistManager::Instance();

    // --- 1. Mundo ---
    G4double world_size = 1.0 * m;
    G4Material *world_mat = nist->FindOrBuildMaterial("G4_AIR"); // Aquí se modifica el material del mundo
    auto solidWorld = new G4Box("World", world_size, world_size, world_size);
    auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
    auto physWorld = new G4PVPlacement(0, {}, logicWorld, "World", 0, false, 0);
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible()); // Invisible

    // --- 2. Materiales absorbentes ---
    G4Material *absorber_mat = DefineMaterials(); // nist->FindOrBuildMaterial("G4_WATER"); -> Material absorbente
    if (!absorber_mat)
    {
        G4cerr << "Fatal: absorber_mat es NULL. Usando G4_WATER temporalmente." << G4endl;
        absorber_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    }
    G4double absorber_thickness = thickness; // 5 cm -> espesor del material absorbente
    auto solidAbs = new G4Box("Absorber", 10 * cm, 10 * cm, absorber_thickness / 2.0);
    auto logicAbs = new G4LogicalVolume(solidAbs, absorber_mat, "Absorber");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicAbs, "Absorber", logicWorld, false, 0);

    // Colores segun material
    G4VisAttributes *visAbs;
    if (materialType == "water")
        visAbs = new G4VisAttributes(G4Colour(0, 0, 1, 0.4)); // Azul
    else if (materialType == "muscle")
        visAbs = new G4VisAttributes(G4Colour(1, 0.8, 0.6, 0.4)); // Color carne
    else if (materialType == "bone")
        visAbs = new G4VisAttributes(G4Colour(1, 1, 0.8, 0.4)); // Color hueso
    else if (materialType == "lead")
        visAbs = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.4)); // Gris
    else if (materialType == "concrete")
        visAbs = new G4VisAttributes(G4Colour(0.6, 0.6, 0.6, 0.4)); // Gris claro
    else
        visAbs = new G4VisAttributes(G4Colour(0, 0, 1, 0.4)); // Azul por defecto
    logicAbs->SetVisAttributes(visAbs);

    // --- 3. Detector ---
    G4Material *detector_mat = nist->FindOrBuildMaterial("G4_AIR"); // Material del detector
    auto solidDet = new G4Box("Detector", 15 * cm, 15 * cm, 2 * mm);
    auto logicDet = new G4LogicalVolume(solidDet, detector_mat, "Detector");
    new G4PVPlacement(0, G4ThreeVector(0, 0, absorber_thickness / 2.0 + 5 * cm),
                      logicDet, "Detector", logicWorld, false, 0);
    auto visDet = new G4VisAttributes(G4Colour(1, 0, 0, 0.6)); // Rojo
    logicDet->SetVisAttributes(visDet);

    // --- 4. Detector lógico: sensitivedetector. ---
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sd = sdManager->FindSensitiveDetector("MyDetectorSD", false);
    if (!sd)
    {
        sd = new MiSensitiveDetector("MyDetectorSD");
        sdManager->AddNewDetector(sd);
    }
    logicDet->SetSensitiveDetector(sd);

    return physWorld;
}
