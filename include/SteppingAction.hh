#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction; // Declaración adelantada
class G4LogicalVolume;

class SteppingAction : public G4UserSteppingAction
{
  public:
    // Recibe EventAction en el constructor
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);

  private:
    EventAction* fEventAction;
    G4LogicalVolume* fScintillatorLV;
};

#endif
