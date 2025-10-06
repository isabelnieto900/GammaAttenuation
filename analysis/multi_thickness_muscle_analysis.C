/* ------- ANÁLISIS MULTI-ESPESOR -------
 * Análisis de atenuación gamma vs espesor para músculo
 * Estudio de la Ley de Beer-Lambert
 * Material: Músculo esquelético
 * Autor: Isabel Nieto, PoPPop21
 * Fecha: Octubre 2025
 * --------------------------------------
 * Uso: root -l -b -q "multi_thickness_muscle_analysis.C"
 */

void multi_thickness_muscle_analysis()
{
    // Configuración de estilo ROOT
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetPadGridX(true);
    gStyle->SetPadGridY(true);

    // Parámetros del análisis
    const int nPoints = 8;
    double thickness[nPoints] = {0.5, 1.0, 2.0, 3.0, 5.0, 7.5, 10.0, 15.0}; // cm
    double transmission[nPoints] = {0};
    double lnTransmission[nPoints] = {0};
    double errors[nPoints] = {0};

    // Material: músculo
    const char *material = "muscle";

    // Crear directorio de salida si no existe
    gSystem->Exec("mkdir -p /home/sofi/REPOS/GammaAttenuation/results/multi_thickness");

    printf("\nAnalisis Multi-Espesor - Músculo Esquelético\n");
    printf("===========================================\n");

    // Recolectar datos de archivos ROOT
    for (int i = 0; i < nPoints; i++)
    {
        TString filename = Form("/home/sofi/REPOS/GammaAttenuation/results/data_thickness_%s_%.1f.root", material, thickness[i]);

        TFile *file = TFile::Open(filename);
        if (!file || file->IsZombie())
        {
            printf("ERROR: No se puede abrir %s\n", filename.Data());
            continue;
        }

        // Leer tree de datos
        TTree *tree = (TTree *)file->Get("data");
        if (!tree)
        {
            printf("ERROR: No se encuentra data tree en %s\n", filename.Data());
            file->Close();
            continue;
        }

        // Leer datos agregados directamente del tree
        Float_t transmissionRatio;
        tree->SetBranchAddress("transmissionRatio", &transmissionRatio);
        tree->GetEntry(0); // Solo hay una entrada por archivo

        transmission[i] = transmissionRatio;
        lnTransmission[i] = log(transmission[i]);
        errors[i] = sqrt(transmissionRatio * 100000) / 100000; // Error aproximado de Poisson

        printf("Espesor %.1f cm: T = %.4f +/- %.4f\n",
               thickness[i], transmission[i], errors[i]);

        file->Close();
    }

    // Crear archivo CSV con resultados
    FILE *csvFile = fopen("/home/sofi/REPOS/GammaAttenuation/results/multi_thickness/thickness_muscle_analysis_data.csv", "w");
    fprintf(csvFile, "Thickness_cm,Transmission,Ln_Transmission,Error\n");
    for (int i = 0; i < nPoints; i++)
    {
        fprintf(csvFile, "%.1f,%.6f,%.6f,%.6f\n",
                thickness[i], transmission[i], lnTransmission[i], errors[i]);
    }
    fclose(csvFile);

    // Ajuste lineal de Beer-Lambert: ln(I/I0) = -μx
    TGraphErrors *graph = new TGraphErrors(nPoints, thickness, lnTransmission, 0, errors);
    TF1 *fitFunc = new TF1("beer_lambert", "[0] + [1]*x", 0, 16);
    fitFunc->SetParameter(0, 0);
    fitFunc->SetParameter(1, -0.08);
    fitFunc->SetParNames("Ln(I0)", "Slope (-μ)");

    graph->Fit(fitFunc, "Q");

    // Extraer coeficiente de atenuación
    double mu_measured = -fitFunc->GetParameter(1);
    double mu_error = fitFunc->GetParError(1);
    double chi2 = fitFunc->GetChisquare();
    double ndf = fitFunc->GetNDF();
    double r2 = 1.0 - (chi2 / graph->GetRMS(2));

    printf("\nResultados del Ajuste Beer-Lambert:\n");
    printf("μ medido = %.4f +/- %.4f cm⁻¹\n", mu_measured, mu_error);
    printf("χ²/ndf = %.2f/%d = %.3f\n", chi2, (int)ndf, chi2 / ndf);
    printf("R² = %.4f\n", r2);

    // Guardar resultados de ajuste
    FILE *resultsFile = fopen("/home/sofi/REPOS/GammaAttenuation/results/multi_thickness/fit_muscle_results.txt", "w");
    fprintf(resultsFile, "Multi-Thickness Analysis Results\n");
    fprintf(resultsFile, "==============================\n");
    fprintf(resultsFile, "Material: Skeletal Muscle\n");
    fprintf(resultsFile, "Energy: 662 keV (Cs-137)\n");
    fprintf(resultsFile, "Thickness range: %.1f - %.1f cm\n", thickness[0], thickness[nPoints - 1]);
    fprintf(resultsFile, "\nBeer-Lambert Fit Results:\n");
    fprintf(resultsFile, "μ = %.4f +/- %.4f cm⁻¹\n", mu_measured, mu_error);
    fprintf(resultsFile, "χ²/ndf = %.3f\n", chi2 / ndf);
    fprintf(resultsFile, "R² = %.4f\n", r2);
    fprintf(resultsFile, "\nHalf-value layer: %.2f cm\n", log(2) / mu_measured);
    fprintf(resultsFile, "Tenth-value layer: %.2f cm\n", log(10) / mu_measured);
    fclose(resultsFile);

    printf("\nDatos guardados en:\n");
    printf("- results/multi_thickness/thickness_muscle_analysis_data.csv\n");
    printf("- results/multi_thickness/fit_muscle_results.txt\n");
    printf("\nAnalisis completado. Ejecutar Python para graficas.\n");
}