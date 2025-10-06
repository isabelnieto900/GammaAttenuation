#!/usr/bin/env python3
"""
Visualización del análisis multi-energía
Comparación GEANT4 vs NIST para coeficientes de atenuación másicos
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# Configuración para ejecución sin display
import matplotlib
matplotlib.use('Agg')

def plot_energy_spectrum():
    """Crear gráfica de comparación GEANT4 vs NIST a múltiples energías"""
    
    print("=== Visualización Multi-Energy Analysis ===")
    
    # Leer datos del análisis
    csv_file = "results/multi_energy/energy_spectrum_comparison.csv"
    
    if not os.path.exists(csv_file):
        print(f"Error: No se encuentra el archivo {csv_file}")
        return
    
    try:
        df = pd.read_csv(csv_file)
        print(f"Datos cargados: {len(df)} puntos de energía")
        
        # Crear figura con subplots
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
        
        # Gráfica 1: Comparación completa log-log
        ax1.loglog(df['Energy_MeV'], df['MuRho_NIST_cm2g'], 'o-', 
                  color='blue', label='NIST', markersize=4, linewidth=2)
        ax1.loglog(df['Energy_MeV'], df['MuRho_GEANT4_cm2g'], 's-', 
                  color='red', label='GEANT4', markersize=4, linewidth=2, alpha=0.8)
        
        # Resaltar punto de 662 keV
        idx_662 = df[abs(df['Energy_keV'] - 662.0) < 1.0].index
        if len(idx_662) > 0:
            i = idx_662[0]
            ax1.loglog(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_NIST_cm2g'], 
                      'o', color='darkblue', markersize=8, markeredgecolor='black')
            ax1.loglog(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g'], 
                      's', color='darkred', markersize=8, markeredgecolor='black')
        
        ax1.set_xlabel('Energía del fotón (MeV)', fontsize=12)
        ax1.set_ylabel('μ/ρ (cm²/g)', fontsize=12)
        ax1.set_title('Coeficientes de Atenuación Másico del Agua\n(Espectro Completo)', fontweight='bold')
        ax1.grid(True, which="both", ls="--", alpha=0.6)
        ax1.legend()
        ax1.set_xlim(1e-3, 2e1)
        ax1.set_ylim(1e-2, 1e4)
        
        # Gráfica 2: Zoom en región de interés (0.1-10 MeV)
        mask_zoom = (df['Energy_MeV'] >= 0.1) & (df['Energy_MeV'] <= 10.0)
        df_zoom = df[mask_zoom]
        
        ax2.loglog(df_zoom['Energy_MeV'], df_zoom['MuRho_NIST_cm2g'], 'o-', 
                  color='blue', label='NIST', markersize=5, linewidth=2)
        ax2.loglog(df_zoom['Energy_MeV'], df_zoom['MuRho_GEANT4_cm2g'], 's-', 
                  color='red', label='GEANT4', markersize=5, linewidth=2, alpha=0.8)
        
        # Resaltar 662 keV en zoom
        if len(idx_662) > 0:
            i = idx_662[0]
            ax2.loglog(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_NIST_cm2g'], 
                      'o', color='darkblue', markersize=10, markeredgecolor='black')
            ax2.loglog(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g'], 
                      's', color='darkred', markersize=10, markeredgecolor='black')
            ax2.annotate(f'662 keV\n(Cs-137)', 
                        xy=(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_NIST_cm2g']),
                        xytext=(1.0, 0.15), fontsize=10, ha='center',
                        arrowprops=dict(arrowstyle='->', color='black', alpha=0.7))
        
        ax2.set_xlabel('Energía del fotón (MeV)', fontsize=12)
        ax2.set_ylabel('μ/ρ (cm²/g)', fontsize=12)
        ax2.set_title('Región de Interés (0.1-10 MeV)', fontweight='bold')
        ax2.grid(True, which="both", ls="--", alpha=0.6)
        ax2.legend()
        ax2.set_xlim(0.1, 10.0)
        
        # Gráfica 3: Diferencias porcentuales
        ax3.semilogx(df['Energy_MeV'], df['Difference_percent'], 'o-', 
                    color='green', markersize=4, linewidth=2)
        
        # Líneas de referencia
        ax3.axhline(y=0, color='black', linestyle='-', alpha=0.5)
        ax3.axhline(y=10, color='orange', linestyle='--', alpha=0.7, label='±10%')
        ax3.axhline(y=-10, color='orange', linestyle='--', alpha=0.7)
        ax3.axhline(y=20, color='red', linestyle='--', alpha=0.7, label='±20%')
        ax3.axhline(y=-20, color='red', linestyle='--', alpha=0.7)
        
        # Resaltar 662 keV
        if len(idx_662) > 0:
            i = idx_662[0]
            ax3.semilogx(df.loc[i, 'Energy_MeV'], df.loc[i, 'Difference_percent'], 
                        'o', color='darkgreen', markersize=10, markeredgecolor='black')
        
        ax3.set_xlabel('Energía del fotón (MeV)', fontsize=12)
        ax3.set_ylabel('Diferencia GEANT4 vs NIST (%)', fontsize=12)
        ax3.set_title('Diferencias Porcentuales', fontweight='bold')
        ax3.grid(True, which="both", ls="--", alpha=0.6)
        ax3.legend()
        ax3.set_xlim(1e-3, 2e1)
        
        # Gráfica 4: Tabla de valores clave
        ax4.axis('tight')
        ax4.axis('off')
        
        # Seleccionar energías clave para mostrar
        key_energies = [0.001, 0.01, 0.1, 0.662, 1.0, 10.0]
        table_data = []
        
        for energy in key_energies:
            idx = df[abs(df['Energy_MeV'] - energy) < energy*0.1].index
            if len(idx) > 0:
                i = idx[0]
                table_data.append([
                    f"{df.loc[i, 'Energy_MeV']:.3f}",
                    f"{df.loc[i, 'Energy_keV']:.0f}",
                    f"{df.loc[i, 'MuRho_NIST_cm2g']:.4f}",
                    f"{df.loc[i, 'MuRho_GEANT4_cm2g']:.4f}",
                    f"{df.loc[i, 'Difference_percent']:+.1f}%"
                ])
        
        table = ax4.table(cellText=table_data,
                         colLabels=['Energía\n(MeV)', 'Energía\n(keV)', 'NIST\n(cm²/g)', 
                                   'GEANT4\n(cm²/g)', 'Diferencia\n(%)'],
                         cellLoc='center',
                         loc='center')
        table.auto_set_font_size(False)
        table.set_fontsize(10)
        table.scale(1.2, 1.5)
        ax4.set_title('Valores Representativos', fontweight='bold')
        
        plt.tight_layout()
        
        # Guardar la gráfica
        output_file = "results/multi_energy/energy_spectrum_analysis.png"
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Gráfica principal guardada: {output_file}")
        
        # Crear gráfica adicional estilo NIST (solo curvas)
        plt.figure(figsize=(10, 8))
        plt.loglog(df['Energy_MeV'], df['MuRho_NIST_cm2g'], 'o-', 
                  color='blue', label='NIST', markersize=4, linewidth=2)
        plt.loglog(df['Energy_MeV'], df['MuRho_GEANT4_cm2g'], 's-', 
                  color='red', label='GEANT4', markersize=4, linewidth=2, alpha=0.8)
        
        # Resaltar punto de 662 keV
        if len(idx_662) > 0:
            i = idx_662[0]
            plt.loglog(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_NIST_cm2g'], 
                      'o', color='darkblue', markersize=10, markeredgecolor='black')
            plt.loglog(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_GEANT4_cm2g'], 
                      's', color='darkred', markersize=10, markeredgecolor='black')
            plt.annotate(f'662 keV (Cs-137)', 
                        xy=(df.loc[i, 'Energy_MeV'], df.loc[i, 'MuRho_NIST_cm2g']),
                        xytext=(2.0, 0.2), fontsize=12, ha='center',
                        arrowprops=dict(arrowstyle='->', color='black', alpha=0.8))
        
        plt.xlabel('Energía del fotón (MeV)', fontsize=14)
        plt.ylabel('Coeficiente de atenuación másico μ/ρ (cm²/g)', fontsize=14)
        plt.title('Coeficientes de Atenuación Másico del Agua\nComparación GEANT4 vs NIST', 
                 fontsize=16, fontweight='bold')
        plt.grid(True, which="both", ls="--", alpha=0.6)
        plt.legend(fontsize=12)
        plt.xlim(1e-3, 2e1)
        plt.ylim(1e-2, 1e4)
        
        plt.tight_layout()
        
        output_file2 = "results/multi_energy/nist_style_comparison.png"
        plt.savefig(output_file2, dpi=300, bbox_inches='tight')
        print(f"Gráfica estilo NIST guardada: {output_file2}")
        
        # Mostrar estadísticas
        print(f"\n=== ESTADÍSTICAS ===")
        print(f"Puntos analizados: {len(df)}")
        print(f"Rango de energía: {df['Energy_MeV'].min():.3f} - {df['Energy_MeV'].max():.1f} MeV")
        print(f"Diferencia promedio: {df['Difference_percent'].mean():+.1f}%")
        print(f"Diferencia máxima: {df['Difference_percent'].max():+.1f}%")
        print(f"Diferencia mínima: {df['Difference_percent'].min():+.1f}%")
        
        if len(idx_662) > 0:
            i = idx_662[0]
            print(f"\n--- Punto de referencia 662 keV ---")
            print(f"NIST:   {df.loc[i, 'MuRho_NIST_cm2g']:.4f} cm²/g")
            print(f"GEANT4: {df.loc[i, 'MuRho_GEANT4_cm2g']:.4f} cm²/g")
            print(f"Diferencia: {df.loc[i, 'Difference_percent']:+.1f}%")
        
    except Exception as e:
        print(f"Error procesando datos: {e}")

if __name__ == "__main__":
    plot_energy_spectrum()