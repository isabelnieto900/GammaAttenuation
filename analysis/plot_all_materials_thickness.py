import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

def parse_fit_results(filepath):
    """
    Lee el archivo de resultados del ajuste y extrae los valores de mu y su error.
    """
    try:
        with open(filepath, 'r') as f:
            for line in f:
                if "Coeficiente de atenuación lineal (mu):" in line:
                    parts = line.split()
                    mu = float(parts[parts.index("(mu):") + 1])
                    mu_error = float(parts[parts.index("+/-") + 1])
                    return mu, mu_error
    except (FileNotFoundError, ValueError, IndexError):
        return None, None
    return None, None

def plot_comparative_attenuation():
    """
    Genera una gráfica comparativa de la atenuación para múltiples materiales.
    """
    print("Generando gráfica comparativa de la Ley de Beer-Lambert...")

    # --- 1. Configuración de materiales y estilos ---
    materials_to_plot = {
        'water': {'label': 'Agua', 'color': 'royalblue', 'marker': 'o'},
        'muscle': {'label': 'Músculo', 'color': 'seagreen', 'marker': 's'},
        'bone': {'label': 'Hueso', 'color': 'firebrick', 'marker': '^'}
    }

    base_path = "results/multi_thickness"
    output_plot_path = os.path.join(base_path, "comparative_beer_lambert_all_materials.png")

    # --- 2. Configurar la gráfica ---
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(12, 8))

    # --- 3. Iterar sobre cada material para graficarlo ---
    for material, style in materials_to_plot.items():
        csv_filepath = os.path.join(base_path, f"thickness_{material}_analysis_data.csv")
        fit_results_filepath = os.path.join(base_path, f"fit_{material}_results.txt")

        if not os.path.exists(csv_filepath):
            print(f"Advertencia: No se encontró el archivo de datos {csv_filepath}. Saltando material '{material}'.")
            continue

        # Cargar datos y resultados del ajuste
        data = pd.read_csv(csv_filepath)
        mu, mu_error = parse_fit_results(fit_results_filepath)

        # Graficar los puntos de la simulación
        ax.errorbar(
            data['Thickness_cm'],
            data['Transmission_I_I0'],
            yerr=data['Transmission_Error'],
            fmt=style['marker'],
            color=style['color'],
            ecolor='lightgray',
            elinewidth=2,
            capsize=4,
            markersize=7,
            label=f"{style['label']} (Simulación)"
        )

        # Graficar la curva teórica del ajuste
        if mu is not None:
            x_fit = np.linspace(0, data['Thickness_cm'].max(), 200)
            y_fit = np.exp(-mu * x_fit)
            
            legend_label = (
                f'$\\mu = {mu:.4f}$ cm$^{{-1}}$ ({style["label"]})'
            )
            
            ax.plot(x_fit, y_fit, color=style['color'], linestyle='--', linewidth=2.5, label=legend_label)

    # --- 4. Estilizar la gráfica final ---
    ax.set_yscale('log')
    ax.set_xlabel('Espesor del Material (cm)', fontsize=14)
    ax.set_ylabel('Transmisión ($I/I_0$)', fontsize=14)
    ax.set_title('Comparativa de Atenuación de Fotones Gamma (662 keV)', fontsize=16, weight='bold')
    
    ax.tick_params(axis='both', which='major', labelsize=12)
    ax.legend(fontsize=11, frameon=True, shadow=True, loc='upper right')
    ax.grid(True, which='both', linestyle='--', linewidth=0.5)
    ax.set_ylim(bottom=1e-2, top=1.1) # Ajustar límites para una mejor visualización

    # --- 5. Guardar la figura ---
    plt.tight_layout()
    fig.savefig(output_plot_path, dpi=300)
    print(f"\n¡Gráfica comparativa guardada con éxito en:\n{output_plot_path}")

if __name__ == "__main__":
    plot_comparative_attenuation()