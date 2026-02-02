#include "PrimaryGeneratorAction.hh"

#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4Event.hh>
#include <G4ios.hh>

PrimaryGeneratorAction::PrimaryGeneratorAction() {
  f_gun = new G4ParticleGun(1);

  auto* particula = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  f_gun->SetParticleDefinition(particula);

  f_gun->SetParticleEnergy(150.*GeV);  // pon aquí lo que estés usando
  f_gun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
  f_gun->SetParticlePosition(G4ThreeVector(0., 0., -10.*cm));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete f_gun; }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* evento) {

  if (evento->GetEventID() == 0) {
    auto* def = f_gun->GetParticleDefinition();
    G4cout
      << "[PrimaryGenerator] particula = " << def->GetParticleName()
      << " (PDG=" << def->GetPDGEncoding() << ")"
      << ", energia = " << f_gun->GetParticleEnergy()/GeV << " GeV"
      << G4endl;
  }

  f_gun->GeneratePrimaryVertex(evento);
}
