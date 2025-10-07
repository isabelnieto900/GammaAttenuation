import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

def parse_fit_results(filepath):
    """
    Lee el archivo de resultados del ajuste y extrae los valores de mu y su error.
    """
    mu = None
    mu_error = None
    try:
        with open(filepath, 'r') as f:
            for line in f:
                if "Coeficiente de atenuación lineal (mu):" in line:
                    parts = line.split()
                    mu_index = parts.index("(mu):") + 1
                    error_index = parts.index("+/-") + 1
                    mu = float(parts[mu_index])
                    mu_error = float(parts[error_index])
                    break
    except FileNotFoundError:
        print(f"Advertencia: No se encontró el archivo de resultados del ajuste: {filepath}")
    except (ValueError, IndexError) as e:
        print(f"Advertencia: No se pudo parsear el archivo de resultados del ajuste: {e}")
    
    return mu, mu_error

def plot_attenuation(material):
    """
    Genera una gráfica de atenuación para un material dado a partir de los datos de análisis.
    """
    print(f"Generando gráfica de atenuación para el material: {material}")

    # --- 1. Definir rutas de archivos ---
    base_path = "results/multi_thickness"
    csv_filepath = os.path.join(base_path, f"thickness_{material}_analysis_data.csv")
    fit_results_filepath = os.path.join(base_path, f"fit_{material}_results.txt")
    output_plot_path = os.path.join(base_path, f"plot_beer_lambert_{material}.png")

    # --- 2. Cargar datos ---
    if not os.path.exists(csv_filepath):
        print(f"Error: No se encontró el archivo de datos CSV: {csv_filepath}")
        print("Asegúrate de haber ejecutado el análisis ROOT primero.")
        return

    data = pd.read_csv(csv_filepath)
    mu, mu_error = parse_fit_results(fit_results_filepath)

    # --- 3. Configurar la gráfica ---
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(10, 7))

    # --- 4. Graficar los datos de la simulación con barras de error ---
    ax.errorbar(
        data['Thickness_cm'],
        data['Transmission_I_I0'],
        yerr=data['Transmission_Error'],
        fmt='o',
        color='royalblue',
        ecolor='lightgray',
        elinewidth=3,
        capsize=5,
        markersize=8,
        label='Datos de Simulación (Geant4)'
    )

    # --- 5. Graficar la curva teórica del ajuste ---
    if mu is not None:
        x_fit = np.linspace(0, data['Thickness_cm'].max(), 200)
        y_fit = np.exp(-mu * x_fit)
        
        legend_label = (
            f'Ajuste Ley de Beer-Lambert\n'
            f'$I/I_0 = e^{{-\\mu x}}$\n'
            f'$\\mu = {mu:.4f} \\pm {mu_error:.4f}$ cm$^{{-1}}$'
        )
        
        ax.plot(x_fit, y_fit, color='crimson', linestyle='--', linewidth=2, label=legend_label)

    # --- 6. Estilizar la gráfica ---
    ax.set_yscale('log')
    ax.set_xlabel('Espesor del Material (cm)', fontsize=14)
    ax.set_ylabel('Transmisión ($I/I_0$)', fontsize=14)
    ax.set_title(f'Atenuación de Fotones Gamma (662 keV) en {material.capitalize()}', fontsize=16, weight='bold')
    
    ax.tick_params(axis='both', which='major', labelsize=12)
    ax.legend(fontsize=12, frameon=True, shadow=True)
    ax.set_ylim(bottom=max(data['Transmission_I_I0'].min() * 0.5, 1e-3)) # Ajustar límite inferior

    # --- 7. Guardar la figura ---
    plt.tight_layout()
    fig.savefig(output_plot_path, dpi=300)
    print(f"Gráfica guardada en: {output_plot_path}")

def main():
    if len(sys.argv) < 2:
        print("Error: Debes especificar un material como argumento.")
        print(f"Uso: python3 {sys.argv[0]} <material>")
        print("Ejemplo: python3 {sys.argv[0]} water")
        sys.exit(1)
    
    material = sys.argv[1].lower()
    plot_attenuation(material)

if __name__ == "__main__":
    main()