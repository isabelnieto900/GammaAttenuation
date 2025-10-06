#!/bin/bash

# Script para ejecutar análisis multi-energía para músculo esquelético
# Genera datos de comparación NIST vs GEANT4 y visualizaciones

echo "=== Análisis Multi-Energía: Músculo Esquelético ==="
echo "Ejecutando análisis de coeficientes de atenuación..."

# Verificar que estamos en el directorio correcto
if [ ! -f "analysis/multi_energy_muscle_analysis.C" ]; then
    echo "Error: No se encuentra el archivo de análisis"
    echo "Ejecute desde el directorio raíz del proyecto GammaAtenuation"
    exit 1
fi

# Crear directorio de resultados
mkdir -p results/multi_energy

# Compilar y ejecutar el análisis C++
echo "Compilando análisis de músculo..."
g++ -o analysis/multi_energy_muscle_analysis analysis/multi_energy_muscle_analysis.C
if [ $? -ne 0 ]; then
    echo "Error en compilación"
    exit 1
fi

echo "Ejecutando análisis de datos..."
./analysis/multi_energy_muscle_analysis

# Verificar que se generó el archivo CSV
if [ ! -f "results/multi_energy/energy_spectrum_muscle_comparison.csv" ]; then
    echo "Error: No se generó el archivo de datos"
    exit 1
fi

# Activar entorno Python y ejecutar visualización
echo "Activando entorno Python..."
source GA/bin/activate

echo "Generando visualizaciones..."
python3 analysis/plot_multi_energy_muscle.py

echo ""
echo "Análisis multi-energía para músculo esquelético completado!"
echo "Resultados en: results/multi_energy/"
echo "Archivos generados:"
echo "   - energy_spectrum_muscle_comparison.csv"
echo "   - energy_spectrum_muscle_analysis.png"
echo "   - nist_style_muscle_comparison.png"

# Limpiar archivo ejecutable
rm -f analysis/multi_energy_muscle_analysis