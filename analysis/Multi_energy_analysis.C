/* ------- ANÁLISIS MULTI-ENERGÍA GENÉRICO -------
 * Análisis de atenuación gamma vs energía para un material específico.
 * Calcula coeficientes de atenuación másicos y los compara con datos NIST.
 *
 * Autor: Isabel Nieto, PoPPop21 (Refactorizado por Gemini Code Assist)
 * Fecha: Octubre 2025
 * --------------------------------------------------------------------
 * Uso: Se invoca desde un script de shell.
 * Ejemplo: root -l -b -q "Multi_energy_analysis.C(\"water\")"
 */

#include <TString.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

void Multi_energy_analysis(const TString &material = "water")
{
    // --- 1. CONFIGURACIÓN INICIAL ---
    gStyle->SetOptStat(0);

    // Crear directorio de salida dinámico
    TString resultsDir = TString::Format("results/%s/multi_energy", material.Data());
    gSystem->Exec(TString::Format("mkdir -p %s", resultsDir.Data()));

    TString materialUpper = material;
    materialUpper.ToUpper();
    printf("\nAnalisis Multi-Energía - %s\n", materialUpper.Data());
    printf("======================================\n");

    // --- 2. DEFINICIÓN DE PARÁMETROS Y DATOS DE REFERENCIA ---

    // Energías de simulación (keV)
    const int nEnergies = 28;
    double energias_keV[nEnergies] = {1, 5, 10, 20, 30, 50, 80, 100, 150, 200, 300, 400, 500, 600, 662, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 6000, 8000, 10000, 15000, 20000};

    // Parámetros físicos y datos NIST
    double density;
    double muRho_NIST[nEnergies];
    bool use_interpolation = false;

    if (material == "water") {
        density = 1.0; // g/cm³
        use_interpolation = true;
    } else if (material == "bone") {
        density = 1.85; // g/cm³ para hueso compacto (densidad usada en los scripts originales)
        // Valores precalculados de μ/ρ para hueso compacto (cm²/g)
        // --- VALORES NIST ---     
        double precalc_vals[nEnergies] = {
            4.881E+03, 1.810E+03, 7.159E+02, 2.459E+02, 1.251E+02, 4.604E+01, 1.848E+01, 1.184E+01, 5.496E+00, 3.491E+00,
            1.910E+00, 1.365E+00, 1.092E+00, 9.351E-01, 8.781E-01, 7.810E-01, 7.001E-01, 6.262E-01, 5.711E-01, 4.931E-01,
            3.991E-01, 3.451E-01, 3.101E-01, 2.851E-01, 2.521E-01, 2.311E-01, 2.001E-01, 1.851E-01};
        std::copy(precalc_vals, precalc_vals + nEnergies, muRho_NIST);
    } else if (material == "muscle") {
        density = 1.05; // g/cm³ para músculo esquelético
        double precalc_vals[nEnergies] = {
            4.125E+03, 1.391E+03, 6.221E+02, 1.942E+02, 8.321E+01, 4.281E+01, 2.479E+01, 1.041E+01, 5.351E+00, 1.681E+00,
            8.121E-01, 3.771E-01, 2.691E-01, 2.281E-01, 2.071E-01, 1.841E-01, 1.711E-01, 1.511E-01, 1.381E-01, 1.191E-01,
            9.711E-02, 8.981E-02, 7.881E-02, 7.091E-02, 6.341E-02, 5.771E-02, 4.961E-02, 3.981E-02};
        std::copy(precalc_vals, precalc_vals + nEnergies, muRho_NIST);
    } else {
        printf("ERROR: Material '%s' no reconocido en el script de análisis. Saliendo.\n", material.Data());
        return;
    }

    // --- 3. INTERPOLACIÓN (solo si es necesario, ej. para agua) ---
    if (use_interpolation) {
        // Tabla de referencia NIST completa para el agua
        const int nNIST = 36;
        double energias_NIST_MeV[nNIST] = {
            1.00000E-03, 1.50000E-03, 2.00000E-03, 3.00000E-03, 4.00000E-03, 5.00000E-03, 6.00000E-03, 8.00000E-03,
            1.00000E-02, 1.50000E-02, 2.00000E-02, 3.00000E-02, 4.00000E-02, 5.00000E-02, 6.00000E-02, 8.00000E-02,
            1.00000E-01, 1.50000E-01, 2.00000E-01, 3.00000E-01, 4.00000E-01, 5.00000E-01, 6.00000E-01, 8.00000E-01,
            1.00000E+00, 1.25000E+00, 1.50000E+00, 2.00000E+00, 3.00000E+00, 4.00000E+00, 5.00000E+00, 6.00000E+00,
            8.00000E+00, 1.00000E+01, 1.50000E+01, 2.00000E+01};
        double muRho_NIST_table[nNIST] = {
            4.078E+03, 1.376E+03, 6.173E+02, 1.929E+02, 8.278E+01, 4.258E+01, 2.464E+01, 1.037E+01,
            5.329E+00, 1.673E+00, 8.096E-01, 3.756E-01, 2.683E-01, 2.269E-01, 2.059E-01, 1.837E-01,
            1.707E-01, 1.505E-01, 1.370E-01, 1.186E-01, 1.061E-01, 9.687E-02, 8.956E-02, 7.865E-02,
            7.072E-02, 6.323E-02, 5.754E-02, 4.942E-02, 3.969E-02, 3.403E-02, 3.031E-02, 2.770E-02,
            2.429E-02, 2.219E-02, 1.941E-02, 1.813E-02};

        for (int i = 0; i < nEnergies; i++) {
            double energy_MeV = energias_keV[i] / 1000.0;
            muRho_NIST[i] = 0.0;

            // Interpolación lineal en escala log-log
            for (int j = 0; j < nNIST - 1; j++) {
                if (energy_MeV >= energias_NIST_MeV[j] && energy_MeV <= energias_NIST_MeV[j + 1]) {
                    double x1 = log(energias_NIST_MeV[j]);
                    double x2 = log(energias_NIST_MeV[j + 1]);
                    double y1 = log(muRho_NIST_table[j]);
                    double y2 = log(muRho_NIST_table[j + 1]);
                    double x = log(energy_MeV);
                    double y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
                    muRho_NIST[i] = exp(y);
                    break;
                }
            }
            // Extrapolación simple para valores fuera de la tabla
            if (muRho_NIST[i] == 0.0) {
                if (energy_MeV < energias_NIST_MeV[0]) muRho_NIST[i] = muRho_NIST_table[0];
                else if (energy_MeV > energias_NIST_MeV[nNIST - 1]) muRho_NIST[i] = muRho_NIST_table[nNIST - 1];
            }
        }
    }

    // --- 4. PROCESAMIENTO DE DATOS DE SIMULACIÓN ---

    double transmission[nEnergies] = {0};
    double muRho_geant4[nEnergies] = {0};
    double mu_geant4[nEnergies] = {0};
    double thickness = 5.0; // cm (constante para este análisis)

    printf("Parámetros de simulación:\n");
    printf("- Material: %s\n", material.Data());
    printf("- Espesor: %.1f cm\n", thickness);
    printf("- Densidad: %.2f g/cm³\n", density);
    printf("\n");

    // Bucle para leer cada archivo .root
    for (int i = 0; i < nEnergies; i++) {
        TString filename = TString::Format("%s/data_energy_%s_%.0fkeV.root", resultsDir.Data(), material.Data(), energias_keV[i]);

        TFile *file = TFile::Open(filename);
        if (!file || file->IsZombie()) {
            printf("ERROR: No se puede abrir %s\n", filename.Data());
            continue;
        }

        TTree *tree = (TTree *)file->Get("data");
        if (!tree) {
            printf("ERROR: No se encuentra el TTree 'data' en %s\n", filename.Data());
            file->Close();
            continue;
        }

        Float_t transmissionRatio;
        tree->SetBranchAddress("transmissionRatio", &transmissionRatio);
        tree->GetEntry(0);

        transmission[i] = transmissionRatio;

        // Calcular coeficiente de atenuación
        if (transmission[i] > 0) {
            // Ley de Beer-Lambert: I = I0 * exp(-μ * x)  =>  μ = -ln(T) / x
            mu_geant4[i] = -log(transmission[i]) / thickness;
            muRho_geant4[i] = mu_geant4[i] / density;
        }

        printf("Energía %.0f keV: T = %.4f, μ/ρ (GEANT4) = %.6f cm²/g\n",
               energias_keV[i], transmission[i], muRho_geant4[i]);

        file->Close();
    }

    // --- 5. GENERACIÓN DE ARCHIVOS DE SALIDA ---

    // Crear archivo CSV con resultados comparativos
    TString csvFileName = TString::Format("%s/comparison.csv", resultsDir.Data());
    FILE *csvFile = fopen(csvFileName.Data(), "w");
    fprintf(csvFile, "Energy_MeV,Energy_keV,MuRho_NIST_cm2g,MuRho_GEANT4_cm2g,Difference_percent\n");

    for (int i = 0; i < nEnergies; i++) {
        double energy_MeV = energias_keV[i] / 1000.0;
        double difference_percent = 0.0;

        if (muRho_NIST[i] > 0 && muRho_geant4[i] > 0) {
            difference_percent = ((muRho_geant4[i] - muRho_NIST[i]) / muRho_NIST[i]) * 100.0;
        }

        fprintf(csvFile, "%.6f,%.1f,%.6e,%.6e,%.2f\n",
                energy_MeV, energias_keV[i], muRho_NIST[i], muRho_geant4[i], difference_percent);
    }
    fclose(csvFile);

    // Encontrar diferencias para 662 keV
    int idx662 = -1;
    for (int i = 0; i < nEnergies; i++) {
        if (abs(energias_keV[i] - 662) < 1) {
            idx662 = i;
            break;
        }
    }

    // Crear archivo de resumen de texto
    TString resultsFileName = TString::Format("%s/analysis_summary.txt", resultsDir.Data());
    FILE *resultsFile = fopen(resultsFileName.Data(), "w");
    fprintf(resultsFile, "Multi-Energy Analysis Summary\n");
    fprintf(resultsFile, "=============================\n");
    fprintf(resultsFile, "Material: %s\n", materialUpper.Data());
    fprintf(resultsFile, "Thickness: %.1f cm\n", thickness);
    fprintf(resultsFile, "Density: %.2f g/cm³\n", density);
    fprintf(resultsFile, "Energy range: %.0f keV - %.0f keV\n", energias_keV[0], energias_keV[nEnergies - 1]);

    if (idx662 != -1 && muRho_geant4[idx662] > 0) {
        double diff_662 = ((muRho_geant4[idx662] - muRho_NIST[idx662]) / muRho_NIST[idx662]) * 100.0;
        fprintf(resultsFile, "\n--- 662 keV Reference Point (Cs-137) ---\n");
        fprintf(resultsFile, "μ/ρ NIST:   %.6f cm²/g\n", muRho_NIST[idx662]);
        fprintf(resultsFile, "μ/ρ GEANT4: %.6f cm²/g\n", muRho_geant4[idx662]);
        fprintf(resultsFile, "Difference: %.2f%%\n", diff_662);
    }
    fclose(resultsFile);

    printf("\nAnálisis completado. Datos guardados en:\n");
    printf("- %s\n", csvFileName.Data());
    printf("- %s\n", resultsFileName.Data());
    printf("\nSiguiente paso: Ejecutar Python para generar gráficas.\n");
}