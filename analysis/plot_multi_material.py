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
    data_file = "/home/sofi/REPOS/GammaAttenuation/results/multi_material/material_comparison.csv"
    
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
    
    print("Visualizaciones Multi-Material generadas:")
    for file in output_files:
        print(f"- {file}")

if __name__ == "__main__":
    plot_material_comparison()