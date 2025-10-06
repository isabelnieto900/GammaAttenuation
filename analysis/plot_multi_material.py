#!/usr/bin/env python3

"""
Visualización del análisis multi-material
Comparación de atenuación gamma entre agua, músculo y hueso
Autor: Isabel Nieto
Fecha: Octubre 2025
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# Configuración para ejecución sin display
import matplotlib
matplotlib.use('Agg')

def plot_material_comparison():
    """Crear visualización de comparación entre materiales"""
    
    print("Generando visualizaciones Multi-Material...")
    
    # Crear directorio de salida
    os.makedirs("results/multi_material", exist_ok=True)
    
    # Archivo de datos CSV
    data_file = "/home/isabel/Physiscs_projects/GammaAtenuation/results/multi_material/material_comparison.csv"
    
    if not os.path.exists(data_file):
        print(f"ERROR: No se encuentra {data_file}")
        return
    
    # Leer datos CSV
    df = pd.read_csv(data_file)
    print(f"Datos cargados: {len(df)} materiales")
    
    # Extraer datos
    materials = df['Material'].values
    transmission = df['Transmission'].values
    mu_values = df['Mu_cm1'].values
    
    # Configurar la figura
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # Colores para cada material
    colors = ['#2E86AB', '#A23B72', '#F18F01']  # Azul, Magenta, Naranja
    material_names = ['Agua', 'Músculo', 'Hueso']
    
    # Gráfica 1: Ratio de transmisión por material
    bars1 = ax1.bar(material_names, transmission, color=colors, alpha=0.8, edgecolor='black')
    
    # Añadir valores en las barras
    for bar, value in zip(bars1, transmission):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                f'{value:.3f}', ha='center', va='bottom', fontweight='bold')
    
    ax1.set_title('Ratio de Transmisión por Material\n(Espesor: 5 cm, Energía: 662 keV)', 
                  fontweight='bold', fontsize=12)
    ax1.set_ylabel('Ratio de Transmisión', fontsize=11)
    ax1.grid(True, alpha=0.3)
    ax1.set_ylim(0, max(transmission) * 1.2)
    
    # Gráfica 2: Coeficiente de atenuación lineal
    bars2 = ax2.bar(material_names, mu_values, color=colors, alpha=0.8, edgecolor='black')
    
    # Añadir valores en las barras
    for bar, value in zip(bars2, mu_values):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.001,
                f'{value:.4f}', ha='center', va='bottom', fontweight='bold')
    
    ax2.set_title('Coeficiente de Atenuación Lineal μ\n(Espesor: 5 cm, Energía: 662 keV)',
                  fontweight='bold', fontsize=12)
    ax2.set_ylabel('μ (cm⁻¹)', fontsize=11)
    ax2.grid(True, alpha=0.3)
    ax2.set_ylim(0, max(mu_values) * 1.2)
    
    # Gráfica 3: Comparación relativa (normalizada al agua)
    water_transmission = transmission[0]  # Primer valor (agua)
    relative_transmission = transmission / water_transmission
    
    bars3 = ax3.bar(material_names, relative_transmission, color=colors, alpha=0.8, edgecolor='black')
    
    # Añadir valores en las barras
    for bar, value in zip(bars3, relative_transmission):
        ax3.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.02,
                f'{value:.2f}x', ha='center', va='bottom', fontweight='bold')
    
    ax3.set_title('Transmisión Relativa\n(Normalizada al Agua)', fontweight='bold', fontsize=12)
    ax3.set_ylabel('Factor Relativo', fontsize=11)
    ax3.grid(True, alpha=0.3)
    ax3.axhline(y=1.0, color='red', linestyle='--', alpha=0.7, label='Referencia (Agua)')
    ax3.legend()
    
    # Gráfica 4: Porcentaje de atenuación
    attenuation_percent = (1 - transmission) * 100
    
    bars4 = ax4.bar(material_names, attenuation_percent, color=colors, alpha=0.8, edgecolor='black')
    
    # Añadir valores en las barras
    for bar, value in zip(bars4, attenuation_percent):
        ax4.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
                f'{value:.1f}%', ha='center', va='bottom', fontweight='bold')
    
    ax4.set_title('Porcentaje de Atenuación\n(100% - Transmisión)', fontweight='bold', fontsize=12)
    ax4.set_ylabel('Atenuación (%)', fontsize=11)
    ax4.grid(True, alpha=0.3)
    ax4.set_ylim(0, max(attenuation_percent) * 1.2)
    
    plt.tight_layout()
    
    # Guardar visualizaciones
    base_name = "results/multi_material/material_comparison_elegant"
    output_files = [f"{base_name}.png", f"{base_name}.pdf", f"{base_name}.svg"]
    
    for output_file in output_files:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
    
    plt.close()
    
    print(f"\nVisualizaciones generadas:")
    for file in output_files:
        print(f"- {file}")

def plot_material_comparison_with_nist():
    """Crear visualización de comparación entre materiales incluyendo valores NIST"""
    
    print("Generando visualizaciones Multi-Material con NIST...")
    
    # Crear directorio de salida
    os.makedirs("results/multi_material", exist_ok=True)
    
    # Datos de la simulación (G4EmStandardPhysics_option4)
    materials = ['Agua', 'Músculo', 'Hueso']
    simulation_mu = [0.0342, 0.0358, 0.0601]  # cm⁻¹
    simulation_transmission = [0.844, 0.839, 0.744]  # Calculados desde mu
    
    # Valores de referencia NIST (narrow beam, 662 keV)
    nist_mu = [0.0828, 0.0781, 0.1311]  # cm⁻¹
    nist_transmission = [np.exp(-mu * 5.0) for mu in nist_mu]  # Para 5 cm de espesor
    
    # Configurar la figura
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))
    
    # Colores para cada material
    colors = ['#2E86AB', '#A23B72', '#F18F01']  # Azul, Magenta, Naranja
    
    # Gráfica 1: Ratio de transmisión por material
    x_pos = np.arange(len(materials))
    width = 0.35
    
    bars1_sim = ax1.bar(x_pos - width/2, simulation_transmission, width, 
                        color=colors, alpha=0.8, edgecolor='black', 
                        label='Simulación (Haz Ancho)')
    bars1_nist = ax1.bar(x_pos + width/2, nist_transmission, width,
                         color='gray', alpha=0.6, edgecolor='black',
                         label='NIST (Haz Angosto)')
    
    # Añadir valores en las barras
    for bar, value in zip(bars1_sim, simulation_transmission):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                f'{value:.3f}', ha='center', va='bottom', fontweight='bold', fontsize=9)
    
    for bar, value in zip(bars1_nist, nist_transmission):
        ax1.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                f'{value:.3f}', ha='center', va='bottom', fontweight='bold', fontsize=9)
    
    ax1.set_title('Ratio de Transmisión por Material\n(Espesor: 5 cm, Energía: 662 keV)', 
                  fontweight='bold', fontsize=12)
    ax1.set_ylabel('Ratio de Transmisión', fontsize=11)
    ax1.set_xticks(x_pos)
    ax1.set_xticklabels(materials)
    ax1.grid(True, alpha=0.3)
    ax1.legend()
    ax1.set_ylim(0, 1.1)
    
    # Gráfica 2: Coeficiente de atenuación lineal μ
    bars2_sim = ax2.bar(x_pos - width/2, simulation_mu, width,
                        color=colors, alpha=0.8, edgecolor='black',
                        label='Simulación (Haz Ancho)')
    bars2_nist = ax2.bar(x_pos + width/2, nist_mu, width,
                         color='gray', alpha=0.6, edgecolor='black',
                         label='NIST (Haz Angosto)')
    
    # Añadir valores en las barras
    for bar, value in zip(bars2_sim, simulation_mu):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.002,
                f'{value:.4f}', ha='center', va='bottom', fontweight='bold', fontsize=9)
    
    for bar, value in zip(bars2_nist, nist_mu):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.002,
                f'{value:.4f}', ha='center', va='bottom', fontweight='bold', fontsize=9)
    
    ax2.set_title('Coeficiente de Atenuación Lineal μ\n(Comparación Simulación vs NIST)',
                  fontweight='bold', fontsize=12)
    ax2.set_ylabel('μ (cm⁻¹)', fontsize=11)
    ax2.set_xticks(x_pos)
    ax2.set_xticklabels(materials)
    ax2.grid(True, alpha=0.3)
    ax2.legend()
    ax2.set_ylim(0, max(max(simulation_mu), max(nist_mu)) * 1.2)
    
    # Gráfica 3: Diferencia porcentual respecto a NIST
    percent_diff = [(sim - nist) / nist * 100 for sim, nist in zip(simulation_mu, nist_mu)]
    
    bars3 = ax3.bar(materials, percent_diff, color=colors, alpha=0.8, edgecolor='black')
    
    # Añadir valores en las barras
    for bar, value in zip(bars3, percent_diff):
        height = bar.get_height()
        y_pos = height - 2 if height < 0 else height + 1
        ax3.text(bar.get_x() + bar.get_width()/2, y_pos,
                f'{value:.1f}%', ha='center', va='top' if height < 0 else 'bottom', 
                fontweight='bold', fontsize=10)
    
    ax3.set_title('Diferencia Relativa vs NIST\n(Valores negativos = Simulación menor que NIST)',
                  fontweight='bold', fontsize=12)
    ax3.set_ylabel('Diferencia Relativa (%)', fontsize=11)
    ax3.grid(True, alpha=0.3)
    ax3.axhline(y=0, color='black', linestyle='-', alpha=0.8, linewidth=2, label='Valor NIST (0%)')
    
    # Añadir texto explicativo
    ax3.text(0.02, 0.98, 'Valores negativos:\nSimulación < NIST', 
             transform=ax3.transAxes, va='top', ha='left',
             bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.2),
             fontsize=9, fontweight='bold')
    
    ax3.legend()
    
    # Gráfica 4: Línea comparativa de coeficientes μ
    x_line = np.arange(len(materials))
    
    line1 = ax4.plot(x_line, simulation_mu, 'o-', color='blue', linewidth=3, 
                     markersize=8, label='Simulación (Haz Ancho)', markerfacecolor='lightblue')
    line2 = ax4.plot(x_line, nist_mu, 's-', color='red', linewidth=3, 
                     markersize=8, label='NIST (Haz Angosto)', markerfacecolor='lightcoral')
    
    # Añadir valores en los puntos
    for i, (sim, nist) in enumerate(zip(simulation_mu, nist_mu)):
        ax4.text(i, sim + 0.005, f'{sim:.4f}', ha='center', va='bottom', 
                fontweight='bold', color='blue', fontsize=10)
        ax4.text(i, nist + 0.005, f'{nist:.4f}', ha='center', va='bottom', 
                fontweight='bold', color='red', fontsize=10)
    
    ax4.set_title('Comparación Directa: μ Simulación vs NIST\n(662 keV, espesor 5 cm)',
                  fontweight='bold', fontsize=12)
    ax4.set_ylabel('μ (cm⁻¹)', fontsize=11)
    ax4.set_xlabel('Material', fontsize=11)
    ax4.set_xticks(x_line)
    ax4.set_xticklabels(materials)
    ax4.grid(True, alpha=0.3)
    ax4.legend()
    ax4.set_ylim(0, max(max(simulation_mu), max(nist_mu)) * 1.15)
    
    plt.tight_layout()
    
    # Guardar visualizaciones
    base_name = "results/multi_material/material_comparison_with_nist"
    output_files = [f"{base_name}.png", f"{base_name}.pdf", f"{base_name}.svg"]
    
    for output_file in output_files:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
    
    plt.close()
    
    # Crear tabla resumen
    print("\n" + "="*80)
    print("COMPARACIÓN SIMULACIÓN vs NIST (662 keV, espesor 5 cm)")
    print("="*80)
    print(f"{'Material':<10} {'Simulación μ':<15} {'NIST μ':<12} {'Diferencia':<12} {'Ratio':<10}")
    print("-"*80)
    
    for i, material in enumerate(materials):
        diff = percent_diff[i]
        ratio = simulation_mu[i] / nist_mu[i]
        print(f"{material:<10} {simulation_mu[i]:<15.4f} {nist_mu[i]:<12.4f} {diff:<12.1f}% {ratio:<10.3f}")
    
    print("\n" + "="*80)
    print("OBSERVACIONES:")
    print("="*80)
    print("• Simulación usa haz ancho + geometría realista (incluye scatter)")
    print("• NIST usa haz angosto + colimación estricta (narrow beam)")
    print("• Diferencias ~54-59% son consistentes con el tipo de geometría")
    print("• Relaciones proporcionales se mantienen correctamente")
    print("• Physics List: G4EmStandardPhysics_option4 (incluye dispersión Rayleigh)")
    print("="*80)
    
    print(f"\nVisualizaciones con NIST generadas:")
    for file in output_files:
        print(f"- {file}")

if __name__ == "__main__":
    plot_material_comparison()
    plot_material_comparison_with_nist()