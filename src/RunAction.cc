#include "RunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction()
: G4UserRunAction()
{
  // Configuración del Analysis Manager
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);

  // --- CORRECCIÓN CLAVE AQUÍ ---
  // Debemos decirle explícitamente que usaremos CSV
  analysisManager->SetDefaultFileType("csv");
  
  // Nombre del archivo de salida (sin extensión)
  analysisManager->SetFileName("EnergyDeposition");

  // Crear Histograma 1D (ID = 0)
  // 100 bins, de 0 a 10 MeV
  analysisManager->CreateH1("Edep", "Energia depositada por evento", 100, 0., 10.*MeV);
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  auto analysisManager = G4AnalysisManager::Instance();
  
  // Reiniciamos el analysis manager para una nueva corrida
  analysisManager->Reset();
  
  // Abrir el archivo
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run*)
{
  auto analysisManager = G4AnalysisManager::Instance();
  
  // Escribir y cerrar
  analysisManager->Write();
  analysisManager->CloseFile();
}
