#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "SensitiveDetector.hh"

G4VPhysicalVolume* DetectorConstruction::Construct() {
    auto nist = G4NistManager::Instance();

    // Material agua
    G4Material* water = nist->FindOrBuildMaterial("G4_WATER");

    // Mundo
    auto solidWorld = new G4Box("World", 20*cm, 20*cm, 20*cm);
    auto logicWorld = new G4LogicalVolume(solidWorld, water, "World");
    auto physWorld  = new G4PVPlacement(0, {}, logicWorld, "World", 0, false, 0);

    // Detector (caja de 10x10x1 cm)
    auto solidDet = new G4Box("Detector", 5*cm, 5*cm, 0.5*cm);
    auto logicDet = new G4LogicalVolume(solidDet, water, "Detector");
    new G4PVPlacement(0, G4ThreeVector(0,0,10*cm), logicDet, "Detector", logicWorld, false, 0);

    // Asignar detector sensible
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sensDet = new SensitiveDetector("GammaDetector");
    sdManager->AddNewDetector(sensDet);
    logicDet->SetSensitiveDetector(sensDet);

    return physWorld;
}
