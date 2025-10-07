// Script de análisis ROOT genérico para el estudio de la ley de Beer-Lambert.
//
// Propósito:
// 1. Procesa los datos de simulación de Geant4 para un material específico.
// 2. Calcula la transmisión de fotones para varios espesores.
// 3. Realiza un ajuste para determinar el coeficiente de atenuación lineal (μ).
// 4. Guarda los resultados en archivos CSV, TXT y ROOT para su posterior visualización.
//
// Uso desde la línea de comandos (como lo hace el script .sh):
//   root -l -b -q 'analysis/Multi_thickness_analysis.C("water")'
//   root -l -b -q 'analysis/Multi_thickness_analysis.C("bone")'

#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <cmath>

// Función principal que se ejecuta. Acepta el nombre del material como argumento.
void Multi_thickness_analysis(const std::string& material = "water") {
    std::cout << "Iniciando análisis de atenuación para el material: " << material << std::endl;

    // --- 1. Configuración ---

    // Mapeo de materiales a sus densidades (g/cm^3) para calcular mu/rho
    std::map<std::string, double> densities;
    densities["water"] = 1.0;
    densities["bone"] = 1.85;
    densities["muscle"] = 1.05;

    if (densities.find(material) == densities.end()) {
        std::cerr << "Error: Material '" << material << "' no reconocido. Densidad desconocida." << std::endl;
        return;
    }
    double density = densities[material];

    // Espesores a analizar (en cm), deben coincidir con los del script .sh
    const std::vector<double> thicknesses = {0.5, 1.0, 2.0, 3.0, 5.0, 7.5, 10.0, 15.0};
    const int n_points = thicknesses.size();

    // Número inicial de eventos en cada simulación
    const double I0 = 100000.0;

    // Vectores para almacenar los datos del gráfico
    std::vector<double> x_values, y_values, ex_values, ey_values;

    // Archivo de salida para los datos tabulados (CSV)
    std::string csv_filename = "results/multi_thickness/thickness_" + material + "_analysis_data.csv";
    std::ofstream csv_file(csv_filename);
    csv_file << "Thickness_cm,Transmitted_I,Transmission_I_I0,Transmission_Error,Neg_Log_Transmission,Neg_Log_Transmission_Error\n";

    // --- 2. Procesamiento de datos de cada simulación ---

    std::cout << "Procesando " << n_points << " archivos ROOT..." << std::endl;

    for (double thickness : thicknesses) {
        // Construir el nombre del archivo de entrada usando stringstream para evitar ceros extra.
        std::stringstream ss;
        // Usar std::fixed y std::setprecision para asegurar que siempre haya un decimal (ej. 1.0, 2.0)
        ss << "results/multi_thickness/data_thickness_" << material << "_" << std::fixed << std::setprecision(1) << thickness << ".root";
        std::string input_filename = ss.str();
        
        TFile* file = TFile::Open(input_filename.c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "  Error: No se pudo abrir el archivo " << input_filename << ". Saltando este punto." << std::endl;
            continue;
        }

        TTree* tree = (TTree*)file->Get("data");
        if (!tree) {
            std::cerr << "  Error: No se encontró el TTree 'data' en " << input_filename << ". Saltando este punto." << std::endl;
            file->Close();
            continue;
        }

        // El número de entradas en el TTree 'Hits' corresponde a los fotones transmitidos
        double I = tree->GetEntries();

        // Calcular la transmisión y su incertidumbre (usando error binomial)
        double transmission = I / I0;
        double transmission_error = sqrt(I * (1.0 - transmission)) / I0;

        // Para el ajuste lineal, usamos -ln(I/I0). La ley de Beer es I = I0*exp(-mu*x)
        // -> ln(I/I0) = -mu*x  -> -ln(I/I0) = mu*x
        // Esto transforma el problema en un ajuste lineal y=m*x, donde m = mu.
        double neg_log_transmission = -log(transmission);
        // Propagación de error: err(ln(y)) = err(y)/y
        double neg_log_transmission_error = transmission_error / transmission;

        // Almacenar valores para el gráfico
        x_values.push_back(thickness);
        y_values.push_back(neg_log_transmission);
        ex_values.push_back(0.0); // Sin error en el espesor
        ey_values.push_back(neg_log_transmission_error);

        // Escribir en el archivo CSV
        csv_file << thickness << "," << I << "," << transmission << "," << transmission_error << "," << neg_log_transmission << "," << neg_log_transmission_error << "\n";

        std::cout << "  Espesor " << thickness << " cm: " << I << " fotones transmitidos." << std::endl;

        file->Close();
    }
    csv_file.close();
    std::cout << "Datos guardados en: " << csv_filename << std::endl;

    // --- 3. Ajuste y visualización ---

    gStyle->SetOptFit(1111); // Mostrar parámetros del ajuste en el gráfico

    TCanvas* canvas = new TCanvas("c1", ("Atenuacion Gamma en " + material).c_str(), 800, 600);
    canvas->SetGrid();

    // Crear el gráfico con errores
    TGraphErrors* graph = new TGraphErrors(x_values.size(), &x_values[0], &y_values[0], &ex_values[0], &ey_values[0]);
    graph->SetTitle(("Atenuacion Gamma en " + material + ";Espesor (cm);-ln(I/I_{0})").c_str());
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kBlue);
    graph->SetLineColor(kBlue);

    // Definir la función de ajuste lineal (y = m*x)
    TF1* fit_func = new TF1("fit_func", "[0]*x", 0, 16);
    fit_func->SetParName(0, "#mu (coef. atenuacion)");
    fit_func->SetParameter(0, 0.1); // Estimación inicial
    fit_func->SetLineColor(kRed);

    // Realizar el ajuste
    graph->Fit(fit_func, "R");
    graph->Draw("AP");

    // Extraer resultados del ajuste
    double mu = fit_func->GetParameter(0);
    double mu_error = fit_func->GetParError(0);
    double chi2 = fit_func->GetChisquare();
    int ndf = fit_func->GetNDF();

    // --- 4. Guardar resultados ---

    // Guardar el gráfico en un archivo ROOT y PNG
    std::string output_root_filename = "results/multi_thickness/thickness_" + material + "_analysis.root";
    std::string output_png_filename = "results/multi_thickness/thickness_" + material + "_analysis.png";
    canvas->SaveAs(output_root_filename.c_str());
    canvas->SaveAs(output_png_filename.c_str());
    std::cout << "Gráfico guardado en: " << output_png_filename << std::endl;

    // Guardar los resultados del ajuste en un archivo de texto
    std::string fit_results_filename = "results/multi_thickness/fit_" + material + "_results.txt";
    std::ofstream fit_file(fit_results_filename);
    fit_file << "Resultados del ajuste para el material: " << material << std::endl;
    fit_file << "--------------------------------------------------" << std::endl;
    fit_file << "Ley de Beer-Lambert: I = I0 * exp(-mu * x)" << std::endl;
    fit_file << "Ajuste lineal realizado sobre: -ln(I/I0) = mu * x" << std::endl;
    fit_file << "--------------------------------------------------" << std::endl;
    fit_file << "Coeficiente de atenuación lineal (mu): " << mu << " +/- " << mu_error << " cm^-1" << std::endl;
    fit_file << "Coeficiente de atenuación másico (mu/rho): " << mu / density << " +/- " << mu_error / density << " cm^2/g" << std::endl;
    fit_file << "Densidad del material (rho): " << density << " g/cm^3" << std::endl;
    fit_file << "Chi^2 / NDF: " << chi2 << " / " << ndf << " = " << chi2 / ndf << std::endl;
    fit_file.close();
    std::cout << "Resultados del ajuste guardados en: " << fit_results_filename << std::endl;

    std::cout << "Análisis para '" << material << "' completado." << std::endl;
}
