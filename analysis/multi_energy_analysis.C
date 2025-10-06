/* ------- ANÁLISIS MULTI-ENERGÍA -------
 * Análisis de atenuación gamma vs energía para agua
 * Estudio de coeficientes de atenuación másicos
 * Material: Agua (H2O)
 * Autor: Isabel Nieto, PoPPop21
 * Fecha: Octubre 2025
 * --------------------------------------
 * Uso: root -l -b -q "multi_energy_analysis.C"
 */

void multi_energy_analysis()
{
    // Configuración de estilo ROOT
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

    // Crear directorio de salida si no existe
    gSystem->Exec("mkdir -p results/multi_energy");

    printf("\nAnalisis Multi-Energía - Agua\n");
    printf("==============================\n");

    // Datos NIST reales para el agua líquida (coeficiente de atenuación másico μ/ρ en cm²/g)
    // Fuente: NIST XCOM Database - Water, Liquid
    // Datos extraídos de la tabla oficial NIST

    // Energías de la tabla NIST (MeV) - Tabla oficial completa
    const int nNIST = 36;
    double energias_NIST_MeV[nNIST] = {
        1.00000E-03, 1.50000E-03, 2.00000E-03, 3.00000E-03, 4.00000E-03, 5.00000E-03, 6.00000E-03, 8.00000E-03,
        1.00000E-02, 1.50000E-02, 2.00000E-02, 3.00000E-02, 4.00000E-02, 5.00000E-02, 6.00000E-02, 8.00000E-02,
        1.00000E-01, 1.50000E-01, 2.00000E-01, 3.00000E-01, 4.00000E-01, 5.00000E-01, 6.00000E-01, 8.00000E-01,
        1.00000E+00, 1.25000E+00, 1.50000E+00, 2.00000E+00, 3.00000E+00, 4.00000E+00, 5.00000E+00, 6.00000E+00,
        8.00000E+00, 1.00000E+01, 1.50000E+01, 2.00000E+01};

    // Coeficientes de atenuación másicos NIST μ/ρ (cm²/g) - Solo coeficiente másico, NO energético
    // Valores exactos de la segunda columna de la tabla NIST oficial
    double muRho_NIST_table[nNIST] = {
        4.078E+03, 1.376E+03, 6.173E+02, 1.929E+02, 8.278E+01, 4.258E+01, 2.464E+01, 1.037E+01,
        5.329E+00, 1.673E+00, 8.096E-01, 3.756E-01, 2.683E-01, 2.269E-01, 2.059E-01, 1.837E-01,
        1.707E-01, 1.505E-01, 1.370E-01, 1.186E-01, 1.061E-01, 9.687E-02, 8.956E-02, 7.865E-02,
        7.072E-02, 6.323E-02, 5.754E-02, 4.942E-02, 3.969E-02, 3.403E-02, 3.031E-02, 2.770E-02,
        2.429E-02, 2.219E-02, 1.941E-02, 1.813E-02};

    // Energías para análisis (keV)
    const int nEnergies = 28;
    double energias_keV[nEnergies] = {1, 5, 10, 20, 30, 50, 80, 100, 150, 200, 300, 400, 500, 600, 662, 800, 1000, 1250, 1500, 2000, 3000, 4000, 5000, 6000, 8000, 10000, 15000, 20000};

    // Interpolar datos NIST para las energías específicas
    double muRho_NIST[nEnergies];
    for (int i = 0; i < nEnergies; i++)
    {
        double energy_MeV = energias_keV[i] / 1000.0;
        muRho_NIST[i] = 0.0;

        // Interpolación lineal en escala log-log
        for (int j = 0; j < nNIST - 1; j++)
        {
            if (energy_MeV >= energias_NIST_MeV[j] && energy_MeV <= energias_NIST_MeV[j + 1])
            {
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

        // Si no se encuentra en el rango, usar el valor más cercano
        if (muRho_NIST[i] == 0.0)
        {
            if (energy_MeV < energias_NIST_MeV[0])
            {
                muRho_NIST[i] = muRho_NIST_table[0];
            }
            else if (energy_MeV > energias_NIST_MeV[nNIST - 1])
            {
                muRho_NIST[i] = muRho_NIST_table[nNIST - 1];
            }
        }
    }

    double transmission[nEnergies] = {0};
    double muRho_geant4[nEnergies] = {0};
    double mu_geant4[nEnergies] = {0};
    double errors[nEnergies] = {0};

    double thickness = 5.0; // cm
    double density = 1.0;   // g/cm³ para agua

    printf("Parámetros de simulación:\n");
    printf("- Material: Agua (H2O)\n");
    printf("- Espesor: %.1f cm\n", thickness);
    printf("- Densidad: %.1f g/cm³\n", density);
    printf("- Energías: %d puntos\n", nEnergies);
    printf("\n");

    // Recolectar datos de archivos ROOT
    for (int i = 0; i < nEnergies; i++)
    {
        TString filename = Form("results/data_energy_water_%.0fkeV.root", energias_keV[i]);

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
    FILE *csvFile = fopen("results/multi_energy/energy_spectrum_comparison.csv", "w");
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

    // Crear gráfica de comparación
    TGraphErrors *graphNIST = new TGraphErrors();
    TGraphErrors *graphGEANT4 = new TGraphErrors();

    int pointsNIST = 0, pointsGEANT4 = 0;

    for (int i = 0; i < nEnergies; i++)
    {
        double energy_MeV = energias_keV[i] / 1000.0;

        if (muRho_NIST[i] > 0)
        {
            graphNIST->SetPoint(pointsNIST, energy_MeV, muRho_NIST[i]);
            pointsNIST++;
        }

        if (muRho_geant4[i] > 0)
        {
            graphGEANT4->SetPoint(pointsGEANT4, energy_MeV, muRho_geant4[i]);
            graphGEANT4->SetPointError(pointsGEANT4, 0, errors[i]);
            pointsGEANT4++;
        }
    }

    // Análisis estadístico
    printf("\n=== ANÁLISIS ESTADÍSTICO ===\n");
    printf("Puntos NIST: %d\n", pointsNIST);
    printf("Puntos GEANT4: %d\n", pointsGEANT4);

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
    FILE *resultsFile = fopen("results/multi_energy/analysis_results.txt", "w");
    fprintf(resultsFile, "Multi-Energy Analysis Results\n");
    fprintf(resultsFile, "============================\n");
    fprintf(resultsFile, "Material: Water (H2O)\n");
    fprintf(resultsFile, "Thickness: %.1f cm\n", thickness);
    fprintf(resultsFile, "Density: %.1f g/cm³\n", density);
    fprintf(resultsFile, "Energy range: %.0f keV - %.0f keV\n", energias_keV[0], energias_keV[nEnergies - 1]);
    fprintf(resultsFile, "Data points: %d\n", nEnergies);
    fprintf(resultsFile, "\nComparison Summary:\n");
    fprintf(resultsFile, "- NIST data points: %d\n", pointsNIST);
    fprintf(resultsFile, "- GEANT4 data points: %d\n", pointsGEANT4);

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
    printf("- results/multi_energy/energy_spectrum_comparison.csv\n");
    printf("- results/multi_energy/analysis_results.txt\n");
    printf("\nAnalisis completado. Ejecutar Python para graficas.\n");
}