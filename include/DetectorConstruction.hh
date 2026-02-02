#pragma once
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  G4VPhysicalVolume* Construct() override;
  G4LogicalVolume* GetScintillatorLV() const { return fScintLV; }

private:
  G4LogicalVolume* fScintLV = nullptr;
};
