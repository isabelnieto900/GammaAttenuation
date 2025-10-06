#ifndef PRIMARYGENERATORMESSENGER_HH
#define PRIMARYGENERATORMESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

class PrimaryGeneratorMessenger : public G4UImessenger
{
public:
    PrimaryGeneratorMessenger(PrimaryGeneratorAction *primary);
    virtual ~PrimaryGeneratorMessenger();

    virtual void SetNewValue(G4UIcommand *command, G4String newValue);

private:
    PrimaryGeneratorAction *primaryAction;
    G4UIdirectory *primaryDir;
    G4UIcmdWithADoubleAndUnit *energyCmd;
};

#endif