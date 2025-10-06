// ------ Simulación de atenuación gamma ------
// Función principal 

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

// --- Clases utilizadas ---
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

int main(int argc,char** argv) {
  
  // Gestión de la ejecución
  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  // --- Gestión del núcleo de Geant4 --- 
  G4RunManager* runManager = new G4RunManager();

  // Definición del detector
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Definición de la lista de física
  PhysicsList* physics = new PhysicsList();
  runManager->SetUserInitialization(physics);

  // Definición de la acción primaria
  PrimaryGeneratorAction* primaryGen = new PrimaryGeneratorAction();
  runManager->SetUserAction(primaryGen);

  // Definición de las acciones de usuario
  RunAction* runAction = new RunAction(detector);
  runManager->SetUserAction(runAction);
  
  EventAction* eventAction = new EventAction(runAction);
  runManager->SetUserAction(eventAction);

  // Inicialización del núcleo de Geant4
  runManager->Initialize();

  // Inicialización de la visualización
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();

  // Obtener el gestor de interfaz de usuario
  G4UImanager* UImanager = G4UImanager::GetUIpointer();  

  if (ui) {
    UImanager->ApplyCommand("/control/execute ../mac/init.mac");
    ui->SessionStart();
    delete ui;
  } else {
    // Modo batch:
    G4String command = "/control/execute ";;
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  
  }

  // Liberar memoria
  delete visManager;
  delete runManager;

  return 0;
}