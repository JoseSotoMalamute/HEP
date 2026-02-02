#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"                 // lista de físicas
//#include "QGSP_BERT.hh"
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv) {
  // RunManager (MT si Geant4 fue compilado con threads)
  auto* runManager = G4RunManagerFactory::CreateRunManager();

  // Geometría
  auto* det = new DetectorConstruction();
  runManager->SetUserInitialization(det);

  // Físicas
  auto* physics = new FTFP_BERT();
  //auto* physics = new QGSP_BERT();
  runManager->SetUserInitialization(physics);

  // Acciones
  runManager->SetUserInitialization(new ActionInitialization(det));

  // Inicializar kernel
  runManager->Initialize();

  // UI
  G4UIExecutive* ui = (argc == 1) ? new G4UIExecutive(argc, argv) : nullptr;

  // Visualización
  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  auto* UImanager = G4UImanager::GetUIpointer();

  if (ui) {
    // Modo interactivo: asumo que ejecutas desde build/
    UImanager->ApplyCommand("/control/execute ../macros/vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    // Modo batch: pasar macro por argumento (recomendado: ../macros/run.mac)
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  delete visManager;
  delete runManager;
  return 0;
}

