#!/usr/bin/env python3
"""
Visualización Análisis de atenuación gamma vs espesor para hueso
Beer-Lambert Law analysis

Autor: Isabel Nieto, PoPPop21
Fecha: Octubre 2025
"""

import matplotlib
matplotlib.use('Agg')  # Backend headless

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
from pathlib import Path
import os

# Configuración de estilo
plt.style.use('seaborn-v0_8-whitegrid')
sns.set_palette("husl")

def main():
    print("Generando visualizaciones Multi-Espesor...")
    
    # Crear directorio si no existe
    os.makedirs("results/multi_thickness", exist_ok=True)
    
    # Leer datos de análisis
    data_file = "/home/isabel/GammaAttenuation/results/multi_thickness/thickness_bone_analysis_data.csv"
    results_file = "/home/isabel/GammaAttenuation/results/multi_thickness/fit_bone_results.txt"
    
    if not os.path.exists(data_file):
        print(f"INFO: Datos de multi-espesor para hueso no encontrados.")
        print(f"Para generar estos datos, ejecutar:")
        print(f"./scripts/run_multi_thickness_bone.sh")
        print(f"./build/gammaAtt analysis/multi_thickness_bone_analysis.C")
        return
    
    # Leer datos CSV
    df = pd.read_csv(data_file)
    
    # Leer resultados de ajuste
    mu_measured = 0.0601  # valor actualizado para hueso
    mu_error = 0.0002
    r_squared = 0.99  # valor por defecto, se calculará más abajo
    
    if os.path.exists(results_file):
        with open(results_file, 'r') as f:
            content = f.read()
            lines = content.replace('\\n', '\n').split('\n')
            for line in lines:
                if "μ =" in line:
                    try:
                        parts = line.split("=")[1].strip().split("+/-")
                        mu_measured = float(parts[0].strip())
                        mu_error = float(parts[1].split()[0])
                    except (ValueError, IndexError):
                        print(f"No se pudo parsear μ de: {line}")
                elif "R² =" in line:
                    try:
                        r_val = float(line.split("=")[1].strip())
                        if 0 <= r_val <= 1:
                            r_squared = r_val
                        else:
                            print(f"R² del archivo fuera de rango: {r_val}, calculando manualmente")
                    except (ValueError, IndexError):
                        print(f"No se pudo parsear R² de: {line}")
    
    # Calcular R² manualmente si es necesario
    if r_squared == 0.99:  # valor por defecto
        ln_transmission = df['Ln_Transmission'].values
        thickness = df['Thickness_cm'].values
        
        y_pred = -mu_measured * thickness
        y_mean = np.mean(ln_transmission)
        
        ss_res = np.sum((ln_transmission - y_pred) ** 2)
        ss_tot = np.sum((ln_transmission - y_mean) ** 2)
        r_squared = 1 - (ss_res / ss_tot)
        
        print(f"R² calculado manualmente: {r_squared:.4f}")
    
    # Configurar figura
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle('Análisis Multi-Espesor - Atenuación Gamma en Hueso Compacto', 
                 fontsize=16, fontweight='bold', y=0.98)
    
    # Panel 1: Transmisión vs Espesor
    thickness = df['Thickness_cm'].values
    transmission = df['Transmission'].values
    errors = df['Error'].values
    
    ax1.errorbar(thickness, transmission, yerr=errors, 
                fmt='o', capsize=5, capthick=2, markersize=8,
                color='steelblue', ecolor='darkblue', label='Datos GEANT4')
    
    # Curva teórica Beer-Lambert
    x_theory = np.linspace(0, 16, 100)
    y_theory = np.exp(-mu_measured * x_theory)
    ax1.plot(x_theory, y_theory, '--', color='red', linewidth=2, 
             label=f'Beer-Lambert (μ={mu_measured:.4f} cm⁻¹)')
    
    ax1.set_xlabel('Espesor (cm)', fontsize=12)
    ax1.set_ylabel('Transmisión I/I₀', fontsize=12)
    ax1.set_title('Transmisión vs Espesor', fontsize=14, fontweight='bold')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Panel 2: Ln(Transmisión) vs Espesor (Ajuste lineal)
    ln_transmission = df['Ln_Transmission'].values
    
    ax2.errorbar(thickness, ln_transmission, yerr=errors/transmission, 
                fmt='s', capsize=5, capthick=2, markersize=8,
                color='forestgreen', ecolor='darkgreen', label='ln(I/I₀)')
    
    # Ajuste lineal
    fit_line = -mu_measured * x_theory
    ax2.plot(x_theory, fit_line, '--', color='red', linewidth=2,
             label=f'Ajuste: y = -{mu_measured:.4f}x')
    
    ax2.set_xlabel('Espesor (cm)', fontsize=12)
    ax2.set_ylabel('ln(I/I₀)', fontsize=12)
    ax2.set_title('Análisis Beer-Lambert', fontsize=14, fontweight='bold')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # Panel 3: Coeficiente de atenuación vs espesor
    mu_local = -ln_transmission / thickness
    
    ax3.scatter(thickness, mu_local, s=100, alpha=0.7, color='orange', 
               label='μ local')
    ax3.axhline(y=mu_measured, color='red', linestyle='--', linewidth=2,
               label=f'μ promedio = {mu_measured:.4f} ± {mu_error:.4f} cm⁻¹')
    ax3.fill_between([0, 16], mu_measured-mu_error, mu_measured+mu_error, 
                    alpha=0.2, color='red')
    
    ax3.set_xlabel('Espesor (cm)', fontsize=12)
    ax3.set_ylabel('μ (cm⁻¹)', fontsize=12)
    ax3.set_title('Coeficiente de Atenuación', fontsize=14, fontweight='bold')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    ax3.set_ylim(0, 0.15)
    
    # Panel 4: Información estadística
    ax4.axis('off')
    
    # Valores de referencia
    nist_mu = 0.1311  # cm⁻¹ para hueso a 662 keV (NIST)
    
    stats_text = f"""
    RESULTADOS DEL ANÁLISIS
    =====================
    
    Material: Hueso Compacto
    Energía: 662 keV (Cs-137)
    Rango de espesores: {thickness[0]:.1f} - {thickness[-1]:.1f} cm
    
    AJUSTE BEER-LAMBERT:
    μ = {mu_measured:.4f} ± {mu_error:.4f} cm⁻¹
    R² = {r_squared:.4f}
    
    COMPARACIÓN NIST:
    μ NIST = {nist_mu:.4f} cm⁻¹
    Diferencia = {((mu_measured - nist_mu)/nist_mu)*100:.1f}%
    
    OBSERVACIONES:
    • Simulación haz ancho vs NIST haz angosto
    • Diferencia esperada por geometría
    • Physics List: G4EmStandardPhysics_option4
    """
    
    ax4.text(0.05, 0.95, stats_text, transform=ax4.transAxes, fontsize=11,
             verticalalignment='top', fontfamily='monospace',
             bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))
    
    # Ajustar layout y guardar
    plt.tight_layout()
    
    # Guardar en múltiples formatos
    base_name = "results/multi_thickness/thickness_bone_analysis_elegant"
    plt.savefig(f"{base_name}.png", dpi=300, bbox_inches='tight')
    plt.savefig(f"{base_name}.pdf", bbox_inches='tight')
    plt.savefig(f"{base_name}.svg", bbox_inches='tight')
    
    plt.close()
    
    print("Visualizaciones Multi-Espesor generadas:")
    print(f"- {base_name}.png")
    print(f"- {base_name}.pdf") 
    print(f"- {base_name}.svg")

if __name__ == "__main__":
    main()