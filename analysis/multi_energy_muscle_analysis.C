/* ------- ANÁLISIS MULTI-ENERGÍA -------
 * Análisis de atenuación gamma vs energía para músculo
 * Estudio de coeficientes de atenuación másicos
 * Material: Músculo esquelético (G4_MUSCLE_SKELETAL_ICRP)
 * Autor: Isabel Nieto, PoPPop21
 * Fecha: Octubre 2025
 * --------------------------------------
 * Uso: root -l -b -q "multi_energy_muscle_analysis.C"
 */

void multi_energy_muscle_analysis()
{
    // Configuración de estilo ROOT
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

    // Crear directorio de salida si no existe
    gSystem->Exec("mkdir -p results/multi_energy");

    printf("\nAnalisis Multi-Energía - Músculo Esquelético\n");
    printf("============================================\n");

    // Datos NIST para músculo esquelético (coeficiente de atenuación másico μ/ρ en cm²/g)
    const int nEnergies = 28;
    double energias_keV[nEnergies] = {1, 5, 10, 20, 30, 50, 80, 100, 150, 200, 300, 400, 500, 600, 662, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 6000, 8000, 10000, 15000, 20000};

    // Datos NIST correspondientes para músculo esquelético (interpolados donde sea necesario)
    double muRho_NIST[nEnergies] = {
        3.861E+03, 3.057E+02, 3.820E+01, 5.776E+00, 2.133E+00, 3.982E-01, 9.695E-02, 4.979E-02,
        1.563E-02, 7.567E-03, 3.508E-03, 2.506E-03, 2.119E-03, 1.924E-03, 1.740E-03, 1.596E-03,
        1.406E-03, 1.279E-03, 1.108E-03, 9.914E-04, 9.050E-04, 8.370E-04, 8.013E-04, 7.350E-04,
        6.607E-04, 5.914E-04, 5.378E-04, 4.621E-04};

    double transmission[nEnergies] = {0};
    double muRho_geant4[nEnergies] = {0};
    double mu_geant4[nEnergies] = {0};
    double errors[nEnergies] = {0};

    double thickness = 5.0; // cm
    double density = 1.05;  // g/cm³ para músculo esquelético

    printf("Parámetros de simulación:\n");
    printf("- Material: Músculo esquelético\n");
    printf("- Espesor: %.1f cm\n", thickness);
    printf("- Densidad: %.2f g/cm³\n", density);
    printf("- Energías: %d puntos\n", nEnergies);
    printf("\n");

    // Recolectar datos de archivos ROOT
    for (int i = 0; i < nEnergies; i++)
    {
        TString filename = Form("results/data_energy_muscle_%.0fkeV.root", energias_keV[i]);

        TFile *file = TFile::Open(filename);
        if (!file || file->IsZombie())
        {
            printf("ERROR: No se puede abrir %s\n", filename.Data());
            transmission[i] = 0.0;
            continue;
        }

        // Leer tree de datos
        TTree *tree = (TTree *)file->Get("data");
        if (!tree)
        {
            printf("ERROR: No se encuentra data tree en %s\n", filename.Data());
            file->Close();
            transmission[i] = 0.0;
            continue;
        }

        // Leer datos de transmisión
        Float_t transmissionRatio;
        tree->SetBranchAddress("transmissionRatio", &transmissionRatio);
        tree->GetEntry(0);

        transmission[i] = transmissionRatio;

        // Calcular coeficiente de atenuación
        if (transmission[i] > 0)
        {
            mu_geant4[i] = -log(transmission[i]) / thickness;
            muRho_geant4[i] = mu_geant4[i] / density;
            errors[i] = sqrt(transmissionRatio * 100000) / 100000; // Error aproximado de Poisson
        }
        else
        {
            mu_geant4[i] = 0;
            muRho_geant4[i] = 0;
            errors[i] = 0;
        }

        printf("Energía %.0f keV: T = %.4f, μ/ρ = %.6f cm²/g\n",
               energias_keV[i], transmission[i], muRho_geant4[i]);

        file->Close();
    }

    // Crear archivo CSV con resultados comparativos
    FILE *csvFile = fopen("results/multi_energy/energy_spectrum_muscle_comparison.csv", "w");
    fprintf(csvFile, "Energy_MeV,Energy_keV,MuRho_NIST_cm2g,MuRho_GEANT4_cm2g,Difference_percent\n");

    for (int i = 0; i < nEnergies; i++)
    {
        double energy_MeV = energias_keV[i] / 1000.0;
        double difference_percent = 0.0;

        if (muRho_NIST[i] > 0 && muRho_geant4[i] > 0)
        {
            difference_percent = ((muRho_geant4[i] - muRho_NIST[i]) / muRho_NIST[i]) * 100.0;
        }

        fprintf(csvFile, "%.6f,%.1f,%.6e,%.6e,%.2f\n",
                energy_MeV, energias_keV[i], muRho_NIST[i], muRho_geant4[i], difference_percent);
    }
    fclose(csvFile);

    // Análisis estadístico
    printf("\n=== ANÁLISIS ESTADÍSTICO ===\n");

    // Encontrar diferencias para 662 keV
    int idx662 = -1;
    for (int i = 0; i < nEnergies; i++)
    {
        if (abs(energias_keV[i] - 662) < 1)
        {
            idx662 = i;
            break;
        }
    }

    if (idx662 >= 0 && muRho_geant4[idx662] > 0)
    {
        double diff_662 = ((muRho_geant4[idx662] - muRho_NIST[idx662]) / muRho_NIST[idx662]) * 100.0;
        printf("\n--- Análisis específico para 662 keV (Cs-137) ---\n");
        printf("μ/ρ NIST:   %.6f cm²/g\n", muRho_NIST[idx662]);
        printf("μ/ρ GEANT4: %.6f cm²/g\n", muRho_geant4[idx662]);
        printf("Diferencia: %.1f%%\n", diff_662);
    }

    // Crear archivo de resultados de análisis
    FILE *resultsFile = fopen("results/multi_energy/muscle_analysis_results.txt", "w");
    fprintf(resultsFile, "Multi-Energy Analysis Results - Músculo Esquelético\n");
    fprintf(resultsFile, "==================================================\n");
    fprintf(resultsFile, "Material: Músculo esquelético (G4_MUSCLE_SKELETAL_ICRP)\n");
    fprintf(resultsFile, "Thickness: %.1f cm\n", thickness);
    fprintf(resultsFile, "Density: %.2f g/cm³\n", density);
    fprintf(resultsFile, "Energy range: %.0f keV - %.0f keV\n", energias_keV[0], energias_keV[nEnergies - 1]);
    fprintf(resultsFile, "Data points: %d\n", nEnergies);

    if (idx662 >= 0 && muRho_geant4[idx662] > 0)
    {
        double diff_662 = ((muRho_geant4[idx662] - muRho_NIST[idx662]) / muRho_NIST[idx662]) * 100.0;
        fprintf(resultsFile, "\n662 keV Reference Point:\n");
        fprintf(resultsFile, "- NIST μ/ρ: %.6f cm²/g\n", muRho_NIST[idx662]);
        fprintf(resultsFile, "- GEANT4 μ/ρ: %.6f cm²/g\n", muRho_geant4[idx662]);
        fprintf(resultsFile, "- Difference: %.1f%%\n", diff_662);
    }
    fclose(resultsFile);

    printf("\nDatos guardados en:\n");
    printf("- results/multi_energy/energy_spectrum_muscle_comparison.csv\n");
    printf("- results/multi_energy/muscle_analysis_results.txt\n");
    printf("\nAnalisis completado. Ejecutar Python para graficas.\n");
}