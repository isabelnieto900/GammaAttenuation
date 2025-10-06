#!/bin/bash

# Script para ejecutar análisis multi-energía para hueso compacto
# Genera datos de comparación NIST vs GEANT4 y visualizaciones

echo "=== Análisis Multi-Energía: Hueso Compacto ==="
echo "Ejecutando análisis de coeficientes de atenuación..."

# Verificar que estamos en el directorio correcto
if [ ! -f "analysis/multi_energy_bone_analysis.C" ]; then
    echo "Error: No se encuentra el archivo de análisis"
    echo "Ejecute desde el directorio raíz del proyecto GammaAttenuation"
    exit 1
fi

# Crear directorio de resultados
mkdir -p results/multi_energy

# Compilar y ejecutar el análisis C++
echo "Compilando análisis de hueso..."
g++ -o analysis/multi_energy_bone_analysis analysis/multi_energy_bone_analysis.C
if [ $? -ne 0 ]; then
    echo "Error en compilación"
    exit 1
fi

echo "Ejecutando análisis de datos..."
./analysis/multi_energy_bone_analysis

# Verificar que se generó el archivo CSV
if [ ! -f "results/multi_energy/energy_spectrum_bone_comparison.csv" ]; then
    echo "Error: No se generó el archivo de datos"
    exit 1
fi

# Activar entorno Python y ejecutar visualización
echo "Activando entorno Python..."
source GA/bin/activate

echo "Generando visualizaciones..."
python3 analysis/plot_multi_energy_bone.py

echo ""
echo "Análisis multi-energía para hueso compacto completado!"
echo "Resultados en: results/multi_energy/"
echo "Archivos generados:"
echo "   - energy_spectrum_bone_comparison.csv"
echo "   - multi_energy_bone_analysis.png"
echo "   - multi_energy_bone_analysis.pdf"
echo "   - multi_energy_bone_analysis.svg"

# Limpiar archivo ejecutable
rm -f analysis/multi_energy_bone_analysis