#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(const DetectorConstruction* det)
: G4VUserActionInitialization(),
  fDet(det)
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
  // 1. Generador de partículas (Vital: faltaba en tu versión original)
  SetUserAction(new PrimaryGeneratorAction);

  // 2. Run Action (Maneja archivos)
  SetUserAction(new RunAction);

  // 3. Event Action (Maneja la energía por evento)
  auto eventAction = new EventAction;
  SetUserAction(eventAction);

  // 4. Stepping Action (Detecta los pasos y llena EventAction)
  SetUserAction(new SteppingAction(eventAction));
}
