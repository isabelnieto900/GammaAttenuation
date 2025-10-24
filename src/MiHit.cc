#include "MiHit.hh"


MiHit::MiHit() 
    : G4VHit(), fEdep(0.), fPos(G4ThreeVector()), fTrackID(-1), fParticleID(0) {}
MiHit::~MiHit() {}
