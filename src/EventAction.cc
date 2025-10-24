#include "EventAction.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "MiHit.hh"
#include "G4ios.hh"

EventAction::EventAction(RunAction *runAct)
    : G4UserEventAction(), runAction(runAct)
{
  // outputFile.open("results/event_data.csv", std::ios::out);
}

EventAction::~EventAction()
{
  // if (outputFile.is_open())
  //   outputFile.close();
  G4cout << "EventAction deleted " << this << G4endl;
}

void EventAction::BeginOfEventAction(const G4Event *event)
{
  // nada costoso o dependiente de objetos externos
}

void EventAction::EndOfEventAction(const G4Event *event)
{
  G4int eventID = event->GetEventID();
  G4int detected = 0;

  G4HCofThisEvent *HCE = event->GetHCofThisEvent();
  if (HCE)
  {
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID("DetectorHitsCollection");
    if (hcID >= 0)
    {
      auto hitsCollection = static_cast<MiHitsCollection *>(HCE->GetHC(hcID));
      if (hitsCollection && hitsCollection->GetSize() > 0)
      {
        // Iterar sobre los hits para encontrar un fotón primario no interactuante
        for (G4int i = 0; i < hitsCollection->GetSize(); ++i)
        {
          MiHit *hit = (*hitsCollection)[i];
          // Un fotón primario no interactuante tendrá TrackID = 1 y PDGEncoding = 22 (fotón)
          // que llega al detector. La energía depositada ya no es un criterio.
          if (hit->GetTrackID() == 1 && hit->GetParticleID() == 22)
          {
            detected = 1;
            runAction->AddTransmittedEvent();
            break; // Solo necesitamos contar un fotón transmitido por evento
          }
        }
      }
    }
  }

  // outputFile << eventID << " , " << detected << "\n";
}