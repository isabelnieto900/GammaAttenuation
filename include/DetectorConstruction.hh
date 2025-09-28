#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction() {}
    ~DetectorConstruction() override {}
    G4VPhysicalVolume* Construct() override;
};
