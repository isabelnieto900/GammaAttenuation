#!/usr/bin/env python3
"""
Visualización genérica del análisis multi-energía.
Compara los resultados de GEANT4 vs NIST para los coeficientes de
atenuación másicos de un material específico.

Uso: python3 analysis/Plot_multi_energy.py <material>
Ejemplo: python3 analysis/Plot_multi_energy.py water
"""

import sys
import os
import pandas as pd
import matplotlib.pyplot as plt

# Configuración para permitir la ejecución en sistemas sin interfaz gráfica (servidores, etc.)
import matplotlib
matplotlib.use('Agg')

def plot_analysis(material):
    """
    Crea un conjunto de gráficas comparativas para un material dado.

    Args:
        material (str): El nombre del material a analizar (ej. 'water', 'bone').
    """
    material_title = material.capitalize()
    print(f"\n=== Visualización del Análisis Multi-Energía para: {material_title} ===")

    # --- 1. Definir rutas y cargar datos ---
    results_dir = f"results/{material}/multi_energy"
    csv_file = f"{results_dir}/comparison.csv"

    if not os.path.exists(csv_file):
        print(f"ERROR: No se encuentra el archivo de datos '{csv_file}'.")
        print(f"Asegúrate de haber ejecutado primero './scripts/Run_multi_energy.sh {material}'.")
        return

    try:
        df = pd.read_csv(csv_file)
        print(f"Datos cargados correctamente: {len(df)} puntos de energía.")
    except Exception as e:
        print(f"ERROR: No se pudieron leer los datos del archivo CSV. Causa: {e}")
        return

    # --- 2. Crear figura principal con 4 subplots ---
    fig, axes = plt.subplots(2, 2, figsize=(16, 13))
    fig.suptitle(f'Análisis de Atenuación para {material_title}: GEANT4 vs. NIST', fontsize=18, fontweight='bold')
    ax1, ax2, ax3, ax4 = axes.flatten()

    # --- Gráfica 1: Comparación completa en escala log-log ---
    ax1.loglog(df['Energy_MeV'], df['MuRho_NIST_cm2g'], 'o-', color='blue', label='NIST', markersize=4)
    ax1.loglog(df['Energy_MeV'], df['MuRho_GEANT4_cm2g'], 's-', color='red', label='GEANT4', markersize=4, alpha=0.8)
    ax1.set_xlabel('Energía del Fotón (MeV)')
    ax1.set_ylabel('μ/ρ (cm²/g)')
    ax1.set_title('Espectro Completo de Atenuación')
    ax1.grid(True, which="both", ls="--", alpha=0.5)
    ax1.legend()
    ax1.set_xlim(1e-3, 2e1)
    ax1.set_ylim(1e-2, 1e4)

    # --- Gráfica 2: Zoom en la región de interés (Efecto Compton) ---
    mask_zoom = (df['Energy_MeV'] >= 0.1) & (df['Energy_MeV'] <= 10.0)
    df_zoom = df[mask_zoom]
    ax2.loglog(df_zoom['Energy_MeV'], df_zoom['MuRho_NIST_cm2g'], 'o-', color='blue', label='NIST', markersize=5)
    ax2.loglog(df_zoom['Energy_MeV'], df_zoom['MuRho_GEANT4_cm2g'], 's-', color='red', label='GEANT4', markersize=5, alpha=0.8)
    ax2.set_xlabel('Energía del Fotón (MeV)')
    ax2.set_ylabel('μ/ρ (cm²/g)')
    ax2.set_title('Región de Interés (0.1 - 10 MeV)')
    ax2.grid(True, which="both", ls="--", alpha=0.5)
    ax2.legend()

    # --- Gráfica 3: Diferencia porcentual ---
    ax3.semilogx(df['Energy_MeV'], df['Difference_percent'], 'o-', color='green', markersize=4)
    ax3.axhline(y=0, color='black', linestyle='-', lw=1)
    ax3.axhline(y=5, color='orange', linestyle='--', lw=1, label='±5%')
    ax3.axhline(y=-5, color='orange', linestyle='--', lw=1)
    ax3.fill_between(df['Energy_MeV'], -5, 5, color='orange', alpha=0.1)
    ax3.set_xlabel('Energía del Fotón (MeV)')
    ax3.set_ylabel('Diferencia Relativa (%)')
    ax3.set_title('Diferencia: [(GEANT4 - NIST) / NIST] %')
    ax3.grid(True, which="both", ls="--", alpha=0.5)
    ax3.legend()
    ax3.set_xlim(1e-3, 2e1)
    # Ajustar límites de 'y' para una mejor visualización
    max_diff = max(abs(df['Difference_percent'].min()), abs(df['Difference_percent'].max()))
    ax3.set_ylim(-max_diff*1.1 - 5, max_diff*1.1 + 5)

    # --- Resaltar punto de 662 keV en las gráficas ---
    idx_662 = df.index[abs(df['Energy_keV'] - 662.0) < 1.0].tolist()
    if idx_662:
        i = idx_662[0]
        # Resaltar en gráfica 1
        ax1.plot(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g'], 'o', color='darkred', markersize=8, markeredgecolor='black', label='662 keV')
        # Resaltar en gráfica 2
        ax2.plot(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g'], 'o', color='darkred', markersize=10, markeredgecolor='black')
        ax2.annotate('662 keV\n(Cs-137)',
                     xy=(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g']),
                     xytext=(df.loc[i, 'Energy_MeV']*1.5, df.loc[i, 'MuRho_GEANT4_cm2g']*1.5),
                     arrowprops=dict(arrowstyle='->', color='black'))
        # Resaltar en gráfica 3
        ax3.plot(df.loc[i, 'Energy_MeV'], df.loc[i, 'Difference_percent'], 'o', color='darkgreen', markersize=10, markeredgecolor='black')

    # --- Gráfica 4: Tabla con valores clave ---
    ax4.axis('off')
    key_energies_keV = [10, 100, 662, 1000, 10000]
    table_data = []
    for energy_keV in key_energies_keV:
        row_idx = df.index[abs(df['Energy_keV'] - energy_keV) < 1.0].tolist()
        if row_idx:
            i = row_idx[0]
            table_data.append([
                f"{df.loc[i, 'Energy_keV']:.0f} keV",
                f"{df.loc[i, 'MuRho_NIST_cm2g']:.4f}",
                f"{df.loc[i, 'MuRho_GEANT4_cm2g']:.4f}",
                f"{df.loc[i, 'Difference_percent']:+.2f}%"
            ])

    if table_data:
        table = ax4.table(cellText=table_data,
                          colLabels=['Energía', 'μ/ρ NIST\n(cm²/g)', 'μ/ρ GEANT4\n(cm²/g)', 'Diferencia'],
                          cellLoc='center', loc='center')
        table.auto_set_font_size(False)
        table.set_fontsize(11)
        table.scale(1.0, 1.8)
        ax4.set_title('Valores Representativos', y=0.85)

    # --- 3. Guardar la figura ---
    plt.tight_layout(rect=[0, 0, 1, 0.96]) # Ajustar para el supertítulo
    output_file = f"{results_dir}/analysis_plot.png"
    plt.savefig(output_file, dpi=200)
    print(f"Gráfica de análisis guardada en: {output_file}")
    plt.close(fig) # Liberar memoria

    # --- 4. Crear y guardar gráfica adicional (estilo NIST) ---
    fig_nist, ax_nist = plt.subplots(figsize=(10, 8))
    ax_nist.loglog(df['Energy_MeV'], df['MuRho_NIST_cm2g'], 'o-', color='blue', label='NIST', markersize=4)
    ax_nist.loglog(df['Energy_MeV'], df['MuRho_GEANT4_cm2g'], 's-', color='red', label='GEANT4', markersize=4, alpha=0.8)
    if idx_662:
        i = idx_662[0]
        ax_nist.plot(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g'], 'o', color='darkred', markersize=8, markeredgecolor='black', label='662 keV')

    ax_nist.set_xlabel('Energía del Fotón (MeV)', fontsize=12)
    ax_nist.set_ylabel('Coeficiente de Atenuación Másico μ/ρ (cm²/g)', fontsize=12)
    ax_nist.set_title(f'Coeficientes de Atenuación Másico para {material_title}', fontsize=16, fontweight='bold')
    ax_nist.grid(True, which="both", ls="--", alpha=0.6)
    ax_nist.legend(fontsize=12)
    ax_nist.set_xlim(1e-3, 2e1)
    ax_nist.set_ylim(1e-2, 1e4)

    plt.tight_layout()
    output_file_nist = f"{results_dir}/nist_style_plot.png"
    plt.savefig(output_file_nist, dpi=200)
    print(f"Gráfica estilo NIST guardada en: {output_file_nist}")
    plt.close(fig_nist)

    # --- 5. Imprimir resumen estadístico ---
    print("\n--- Resumen Estadístico de Diferencias ---")
    print(f"Diferencia promedio: {df['Difference_percent'].mean():+.2f}%")
    print(f"Diferencia (desv. est.): {df['Difference_percent'].std():.2f}%")
    if idx_662:
        i = idx_662[0]
        print(f"Diferencia a 662 keV: {df.loc[i, 'Difference_percent']:+.2f}%")

def main():
    """
    Función principal que procesa los argumentos de la línea de comandos.
    """
    if len(sys.argv) < 2:
        print("ERROR: Debes especificar un material como argumento.")
        print("Uso: python3 analysis/Plot_multi_energy.py <material>")
        print("Ejemplos: python3 analysis/Plot_multi_energy.py water")
        sys.exit(1)

    material = sys.argv[1]
    plot_analysis(material)

if __name__ == "__main__":
    main()