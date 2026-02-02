#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScintillatorLV(nullptr)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // Obtener el volumen lógico del centellador (solo una vez)
  if (!fScintillatorLV) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScintillatorLV = detectorConstruction->GetScintillatorLV();
  }

  // Obtener volumen del paso actual
  auto volume = step->GetPreStepPoint()->GetTouchableHandle()
                    ->GetVolume()->GetLogicalVolume();

  // Si el paso ocurre en el centellador, sumamos la energía
  if (volume == fScintillatorLV) {
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep > 0.) {
        fEventAction->AddEdep(edep);
    }
  }
}
