#include "MiSensitiveDetector.hh"
#include "MiHit.hh" 
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"

MiSensitiveDetector::MiSensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name), hitsCollection(nullptr) {
    
    // Aquí registramos el nombre de la colección de hits
    collectionName.insert("DetectorHitsCollection");
}

MiSensitiveDetector::~MiSensitiveDetector() {
    G4cout << "MiSensitiveDetector deleted " << this << G4endl;
}

void MiSensitiveDetector::Initialize(G4HCofThisEvent* hce) {    
    hitsCollection = new MiHitsCollection(SensitiveDetectorName, collectionName[0]);
    
    G4cout << "MiSensitiveDetector::Initialize: SD name= " << SensitiveDetectorName << " collectionName[0]= " << collectionName[0] << G4endl;


    static G4int hcID = -1;
    if (hcID < 0) {
        hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        G4cout << "MiSensitiveDetector: got hcID = " << hcID << G4endl;
    }
    hce->AddHitsCollection(hcID, hitsCollection);
}

G4bool MiSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* /*history*/) {
    auto hit = new MiHit();
    
    // Energía depositada
    hit->SetEdep(step->GetTotalEnergyDeposit());

    // posicion del pre-step point
    hit->SetPos(step->GetPreStepPoint()->GetPosition());


    // Insertamos en la colección 
    hitsCollection->insert(hit);

    return true;
}