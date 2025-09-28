#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGenerator.hh"

int main(int argc, char** argv) {
    // Run manager
    auto* runManager = new G4RunManager;

    // Inicializaciones
    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserAction(new PrimaryGenerator());

    // Inicializar
    runManager->Initialize();

#ifdef G4VIS_USE
    // Visualización
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif

    // UI
    auto* UImanager = G4UImanager::GetUIpointer();

    if (argc > 1) {
        // Modo batch - ejecutar macro desde línea de comandos
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    } else {
        // Modo interactivo
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        
        // Ejecutar macro por defecto si existe
        UImanager->ApplyCommand("/control/execute ../macros/init_vis.mac");
        
        ui->SessionStart();
        delete ui;
    }

    delete runManager;
    return 0;
}
