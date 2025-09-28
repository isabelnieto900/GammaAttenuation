#include "SensitiveDetector.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include <iostream>

SensitiveDetector::SensitiveDetector(const G4String& name)
: G4VSensitiveDetector(name), fEnergyDeposit(0.), fGammaCount(0) {}

void SensitiveDetector::Initialize(G4HCofThisEvent*) {
    fEnergyDeposit = 0.;
    fGammaCount = 0;
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    auto edep = step->GetTotalEnergyDeposit();
    fEnergyDeposit += edep;

    auto particle = step->GetTrack()->GetDefinition();
    if (particle == G4Gamma::GammaDefinition()) {
        fGammaCount++;
    }
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
    G4cout << "Evento terminado: "
           << "E_dep = " << fEnergyDeposit/keV << " keV, "
           << "Gammas detectados = " << fGammaCount << G4endl;
}
