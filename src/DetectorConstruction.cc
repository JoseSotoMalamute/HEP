#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

G4VPhysicalVolume* DetectorConstruction::Construct() {

  auto* nist = G4NistManager::Instance();

  // =========================
  // Materiales
  // =========================
  auto* matWorld = nist->FindOrBuildMaterial("G4_Galactic");
  auto* matScint = nist->FindOrBuildMaterial("G4_POLYSTYRENE");

  // =========================
  // Mundo
  // =========================
  const G4double worldHalf = 20.*cm;

  auto* solidWorld = new G4Box(
    "World",
    worldHalf, worldHalf, worldHalf
  );

  auto* logicWorld = new G4LogicalVolume(
    solidWorld,
    matWorld,
    "WorldLV"
  );

  auto* physWorld = new G4PVPlacement(
    nullptr,
    {},
    logicWorld,
    "World",
    nullptr,
    false,
    0,
    true
  );

  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

  // =========================
  // Centellador (volumen sensible)
  // =========================
  // Espesor total: 0.7 cm → semiespesor 0.35 cm
  const G4double scintHalfZ = 0.35*cm;

  auto* solidScint = new G4Box(
    "Scintillator",
    1.*cm,   // tamaño transversal arbitrario
    1.*cm,
    scintHalfZ
  );

  fScintLV = new G4LogicalVolume(
    solidScint,
    matScint,
    "Scintillator"
  );

  new G4PVPlacement(
    nullptr,
    {},
    fScintLV,
    "ScintillatorPV",
    logicWorld,
    false,
    0,
    true
  );

  // Visualización
  auto* visScint = new G4VisAttributes(G4Colour(0.2, 0.8, 0.2));
  visScint->SetForceSolid(true);
  fScintLV->SetVisAttributes(visScint);

  // =========================
  // Región + Production Cuts (CLAVE)
  // =========================
  auto* scintRegion = new G4Region("ScintRegion");
  scintRegion->AddRootLogicalVolume(fScintLV);

  auto* cuts = new G4ProductionCuts();
  cuts->SetProductionCut(1*um, "e-");
  cuts->SetProductionCut(1*um, "e+");
  cuts->SetProductionCut(1*um, "gamma");

  scintRegion->SetProductionCuts(cuts);

  return physWorld;
}
