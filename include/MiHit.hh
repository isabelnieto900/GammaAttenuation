#ifndef MIHIT_HH
#define MIHIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"
#include "G4Allocator.hh"
#include "globals.hh"

class MiHit : public G4VHit {
public:
    MiHit();
    virtual ~MiHit();

    // Defino metodos obligatorios
    virtual void Draw() {}
    virtual void Print() {}

    //Getters y setters
    void SetEdep(G4double edep) { fEdep = edep; }
    G4double GetEdep() const { return fEdep; }

    void SetPos(const G4ThreeVector& pos) { fPos = pos; }
    G4ThreeVector GetPos() const { return fPos; }

    void SetTrackID(G4int trackID) { fTrackID = trackID; }
    G4int GetTrackID() const { return fTrackID; }

    void SetParticleID(G4int particleID) { fParticleID = particleID; }
    G4int GetParticleID() const { return fParticleID; }
    
    private:
    G4double fEdep;         // Energía depositada
    G4ThreeVector fPos;     // Posición del hit
    G4int fTrackID;         // ID de la pista
    G4int fParticleID;      // ID de la partícula (PDG code)
};

// Definimos la colección de hits como un typedef
using MiHitsCollection = G4THitsCollection<MiHit>;

#endif