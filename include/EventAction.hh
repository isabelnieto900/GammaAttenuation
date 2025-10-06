#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "RunAction.hh"
#include "globals.hh"
#include <fstream>

class EventAction : public G4UserEventAction {
public:
  EventAction(RunAction* runAction);
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

private:
  RunAction* runAction;
  std::ofstream outputFile;
};

#endif // EVENTACTION_HH