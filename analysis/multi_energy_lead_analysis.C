/* ------- ANÁLISIS MULTI-ENERGÍA -------
 * Análisis de atenuación gamma vs energía para plomo
 * Estudio de coeficientes de atenuación másicos
 * Material: Plomo (G4_LEAD_COMPACT_ICRU)
 * Autor: Isabel Nieto, PoPPop21
 * Fecha: Octubre 2025
 * --------------------------------------
 * Uso: root -l -b -q "multi_energy_lead_analysis.C"
 */

void multi_energy_lead_analysis()
{
    // Configuración de estilo ROOT
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

    // Crear directorio de salida si no existe
    gSystem->Exec("mkdir -p results/multi_energy");

    printf("\nAnalisis Multi-Energía - Plomo Compacto\n");
    printf("=======================================\n");

    // Datos NIST para plomo Pbo (coeficiente de atenuación másico μ/ρ en cm²/g)
    const int nEnergies = 28;
    double energias_keV[nEnergies] = {1, 5, 10, 20, 30, 50, 80, 100, 150, 200, 300, 400, 500, 600, 662, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 6000, 8000, 10000, 15000, 20000};

    // Datos NIST correspondientes para plomo Pbo (interpolados donde sea necesario)
    double muRho_NIST[nEnergies] = {
        5.549E+03, 4.398E+02, 5.499E+01, 8.319E+00, 3.072E+00, 5.730E-01, 1.395E-01, 7.161E-02,
        2.248E-02, 1.088E-02, 5.051E-03, 3.609E-03, 3.052E-03, 2.769E-03, 2.505E-03, 2.297E-03,
        2.024E-03, 1.841E-03, 1.596E-03, 1.426E-03, 1.302E-03, 1.204E-03, 1.153E-03, 1.057E-03,
        9.507E-04, 8.511E-04, 7.741E-04, 6.646E-04};

    double transmission[nEnergies] = {0};
    double muRho_geant4[nEnergies] = {0};
    double mu_geant4[nEnergies] = {0};
    double errors[nEnergies] = {0};

    double thickness = 5.0; // cm
    double density = 1.85;  // g/cm³ para plomo Pbo

    printf("Parámetros de simulación:\n");
    printf("- Material: Plomo\n");
    printf("- Espesor: %.1f cm\n", thickness);
    printf("- Densidad: %.2f g/cm³\n", density);
    printf("- Energías: %d puntos\n", nEnergies);
    printf("\n");

    // Recolectar datos de archivos ROOT
    for (int i = 0; i < nEnergies; i++)
    {
        TString filename = Form("results/data_energy_lead_%.0fkeV.root", energias_keV[i]);

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
    FILE *csvFile = fopen("results/multi_energy/energy_spectrum_lead_comparison.csv", "w");
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
    FILE *resultsFile = fopen("results/multi_energy/lead_analysis_results.txt", "w");
    fprintf(resultsFile, "Multi-Energy Analysis Results - Plomo Compacto\n");
    fprintf(resultsFile, "==============================================\n");
    fprintf(resultsFile, "Material: Plomo (G4_LEAD_COMPACT_ICRU)\n");
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
    printf("- results/multi_energy/energy_spectrum_lead_comparison.csv\n");
    printf("- results/multi_energy/lead_analysis_results.txt\n");
    printf("\nAnalisis completado. Ejecutar Python para graficas.\n");
}
}