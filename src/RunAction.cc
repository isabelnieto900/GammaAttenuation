/* ------- GAMMA ATTENUATION SIMULATION -------
Autor: @isabelnieto900, @PoPPop21
Fecha: Octubre 2025
-----------------------------------------------
*/
#include "RunAction.hh"
#include "G4Run.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include <iostream>
#include <fstream>

#ifdef USE_ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TString.h"
#endif

RunAction::RunAction(DetectorConstruction *det)
    : G4UserRunAction(), detector(det), totalEvents(0), transmittedEvents(0)
{
#ifdef USE_ROOT
  rootFile = nullptr;
  attenuationTree = nullptr;
  attenuationHist = nullptr;
  G4cout << "RunAction: ROOT support enabled (datos únicamente)" << G4endl;
#else
  G4cout << "RunAction: ROOT support not available" << G4endl;
#endif
}

RunAction::~RunAction()
{
#ifdef USE_ROOT
  if (rootFile)
  {
    rootFile->Close();
    delete rootFile;
  }

#endif
}

void RunAction::BeginOfRunAction(const G4Run *run)
{
  totalEvents = run->GetNumberOfEventToBeProcessed();
  transmittedEvents = 0;

  std::cout << "=== Comenzando Run " << run->GetRunID() << " ===" << std::endl;
  std::cout << "Material: " << detector->GetMaterial() << std::endl;
  std::cout << "Espesor: " << detector->GetThickness() / CLHEP::cm << " cm" << std::endl;
  std::cout << "Eventos totales: " << totalEvents << std::endl;

#ifdef USE_ROOT
  // Crear archivo ROOT simple
  TString rootFileName = TString::Format("results/data_run_%s.root", detector->GetMaterial().c_str());
  rootFile = new TFile(rootFileName.Data(), "RECREATE");

  // Crear Tree simple para datos
  attenuationTree = new TTree("data", "Attenuation Data");

  // Crear histograma para coeficientes de atenuación
  attenuationHist = new TH1F("attenuationCoeff", "Coeficiente de Atenuacion;Coeficiente (cm^{-1});Frecuencia", 100, 0, 0.2);

  // Variables básicas
  runData.runID = run->GetRunID();
  strncpy(runData.material, detector->GetMaterial().c_str(), 49);
  runData.material[49] = '\0';
  runData.thickness = detector->GetThickness() / CLHEP::cm;
  runData.totalEvents = totalEvents;

  // Solo branches esenciales
  attenuationTree->Branch("runID", &runData.runID, "runID/I");
  attenuationTree->Branch("material", runData.material, "material/C");
  attenuationTree->Branch("thickness", &runData.thickness, "thickness/F");
  attenuationTree->Branch("totalEvents", &runData.totalEvents, "totalEvents/I");
  attenuationTree->Branch("transmittedEvents", &runData.transmittedEvents, "transmittedEvents/I");
  attenuationTree->Branch("transmissionRatio", &runData.transmissionRatio, "transmissionRatio/F");
  attenuationTree->Branch("attenuationCoeff", &runData.attenuationCoeff, "attenuationCoeff/F");

  G4cout << "ROOT: Archivo " << rootFileName << " creado (solo datos)" << G4endl;
#endif

  // Preparar archivo de resultados
  std::ofstream resultsFile("results/results_summary.txt", std::ios::app);
  resultsFile << "\n=== RUN " << run->GetRunID() << " ===\n";
  resultsFile << "Material: " << detector->GetMaterial() << "\n";
  resultsFile << "Espesor: " << detector->GetThickness() / CLHEP::cm << " cm\n";
  resultsFile << "Eventos: " << totalEvents << "\n";
  resultsFile.close();
}

void RunAction::EndOfRunAction(const G4Run *run)
{
  G4double transmissionRatio = (G4double)transmittedEvents / totalEvents;
  G4double attenuationCoeff = (transmittedEvents > 0) ? -std::log(transmissionRatio) / (detector->GetThickness() / CLHEP::cm) : 999.0;

  std::cout << "=== Finalizando Run " << run->GetRunID() << " ===" << std::endl;
  std::cout << "Eventos transmitidos: " << transmittedEvents << std::endl;
  std::cout << "Razón de transmisión: " << transmissionRatio << std::endl;
  std::cout << "Coeficiente de atenuación: " << attenuationCoeff << " cm^-1" << std::endl;

#ifdef USE_ROOT
  // --- DAtos que recolecta ROOT ---
  // Estos datos son los que utilizaremos más adelante en multi_analysis.C
  runData.transmittedEvents = transmittedEvents;
  runData.transmissionRatio = transmissionRatio;
  runData.attenuationCoeff = attenuationCoeff;

  // Llenar Tree
  attenuationTree->Fill();

  // Llenar histograma
  attenuationHist->Fill(attenuationCoeff);

  // Guardar archivo ROOT
  rootFile->cd();
  attenuationTree->Write();
  attenuationHist->Write();
  rootFile->Close();  // Cerramos el archivo aquí. El destructor se encargará de borrarlo.
  // delete rootFile; // No borrar aquí, el destructor de RunAction lo hará.
  rootFile = nullptr; // Evitamos que el destructor intente borrarlo de nuevo

  G4cout << "ROOT: Datos guardados" << G4endl;
  G4cout << "ROOT: Archivo: data_run" << run->GetRunID() << ".root" << G4endl;
  G4cout << "ROOT: Datos guardados en data_run_" << detector->GetMaterial() << ".root" << G4endl;
#endif

  // Guardar resultados finales
  std::ofstream resultsFile("results/results_summary.txt", std::ios::app);
  resultsFile << "Transmitidos: " << transmittedEvents << "\n";
  resultsFile << "Transmisión: " << transmissionRatio << "\n";
  resultsFile << "Coef. atenuación: " << attenuationCoeff << " cm^-1\n";
  resultsFile.close();

  // Archivo CSV para ROOT
  std::ofstream csvFile("results/attenuation_data.csv", std::ios::app);
  csvFile << detector->GetMaterial() << ","
          << detector->GetThickness() / CLHEP::cm << ","
          << totalEvents << ","
          << transmittedEvents << ","
          << transmissionRatio << ","
          << attenuationCoeff << "\n";
  csvFile.close();
}

void RunAction::AddTransmittedEvent()
{
  transmittedEvents++;
}