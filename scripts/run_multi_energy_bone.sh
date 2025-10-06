#!/bin/bash

# Script Multi-Energía para Hueso Compacto
# Ejecuta simulaciones GEANT4 reales para diferentes energías
# Analiza coeficientes de atenuación vs energía

echo "========================================"
echo "  ANÁLISIS MULTI-ENERGÍA: HUESO"
echo "========================================"
echo "Material: Hueso compacto"
echo "Análisis: Coeficientes de atenuación vs energía"
echo "Rango: 1 keV - 20 MeV"
echo ""

# Verificar que ROOT esté disponible
if ! command -v root &> /dev/null; then
    echo "ERROR: ROOT no está instalado"
    exit 1
fi

# Verificar que Python esté disponible
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python3 no está disponible"
    exit 1
fi

# Verificar que estamos en el directorio correcto
if [ ! -f "build/gammaAtt" ]; then
    echo "ERROR: Ejecutable GEANT4 no encontrado en build/gammaAtt"
    echo "Ejecuta: cd build && make"
    exit 1
fi

# Crear directorio de resultados
mkdir -p results/multi_energy

echo "Paso 1: Generando datos de simulación GEANT4..."
echo "----------------------------------------------"

# Energías a simular (en keV)
ENERGIES=(1 5 10 20 30 50 80 100 150 200 300 400 500 600 662 800 1000 1250 1500 2000 3000 4000 5000 6000 8000 10000 15000 20000)

echo "Ejecutando simulaciones GEANT4 para ${#ENERGIES[@]} energías..."

for energy in "${ENERGIES[@]}"; do
    MAC_FILE="mac/energy_bone_${energy}keV.mac"
    DATA_FILE="results/data_energy_bone_${energy}keV.root"
    
    # Verificar si ya existe el archivo de datos
    if [ -f "$DATA_FILE" ]; then
        echo "  Energía ${energy} keV: datos existentes (saltando)"
        continue
    fi
    
    # Crear archivo macro
    echo "  Creando macro para energía ${energy} keV..."
    cat > "$MAC_FILE" << EOF
# Configuración para hueso - ${energy} keV
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial bone
/detector/setThickness 5.0 cm

# Configurar energía del fotón
/gun/setEnergy ${energy} keV

# Inicializar
/run/initialize

# Ejecutar simulación
/run/beamOn 100000
EOF
    
    echo "  Simulando energía ${energy} keV (100k eventos)..."
    cd build
    ./gammaAtt "../$MAC_FILE"
    cd ..
    
    # Verificar que se generó el archivo
    if [ ! -f "$DATA_FILE" ]; then
        echo "  ERROR: No se generó el archivo $DATA_FILE"
        exit 1
    fi
    
    echo "  Energía ${energy} keV: ✓ completada"
done

echo ""
echo "Paso 2: Ejecutando análisis ROOT..."
echo "-----------------------------------"

# Ejecutar análisis ROOT
root -l -b -q "analysis/multi_energy_bone_analysis.C"

echo "Análisis ROOT completado"
echo ""

echo "Paso 3: Generando visualizaciones..."
echo "------------------------------------"

# Activar entorno Python y ejecutar visualización
echo "Activando entorno Python..."
source GA/bin/activate

echo "Generando gráficas..."
python3 analysis/plot_multi_energy_bone.py

echo ""
echo "=========================================="
echo "  ANÁLISIS MULTI-ENERGÍA COMPLETADO"
echo "=========================================="
echo ""
echo "Datos generados en:"
echo "  results/multi_energy/"
echo ""
echo "Archivos generados:"
echo "  - energy_spectrum_bone_comparison.csv"
echo "  - multi_energy_bone_analysis.png"
echo "  - multi_energy_bone_analysis.pdf"
echo "  - multi_energy_bone_analysis.svg"
echo ""