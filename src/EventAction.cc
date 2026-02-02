#include "EventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction()
: G4UserEventAction(),
  fEdep(0.)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
  // Reseteamos la energía al inicio de cada evento (cada muón)
  fEdep = 0.;
}

void EventAction::EndOfEventAction(const G4Event*)
{
  // Al final del evento, guardamos el dato en el histograma/CSV
  auto analysisManager = G4AnalysisManager::Instance();

  // Llenamos el histograma ID 0 (definido en RunAction)
  // Convertimos a MeV
  analysisManager->FillH1(0, fEdep/MeV);
}
