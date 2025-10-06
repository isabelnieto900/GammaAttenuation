#ifndef DETECTORMESSENGER_HH
#define DETECTORMESSENGER_HH

#include "G4UImessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class DetectorMessenger : public G4UImessenger {
public:
    DetectorMessenger(DetectorConstruction* detector);
    virtual ~DetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);

private:
    DetectorConstruction* detectorConstruction;
    
    G4UIdirectory* detectorDir;
    G4UIcmdWithAString* materialCmd;
    G4UIcmdWithADoubleAndUnit* thicknessCmd;
};

#endif // DETECTORMESSENGER_HH