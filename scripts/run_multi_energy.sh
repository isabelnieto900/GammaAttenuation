#!/bin/bash

# Script Multi-Energía para Polietileno
# Ejecuta simulaciones GEANT4 reales para diferentes energías
# Analiza coeficientes de atenuación vs energía

echo "========================================"
echo "  ANÁLISIS MULTI-ENERGÍA: POLIETILENO"
echo "========================================"
echo "Material: Polietileno (HDPE)"
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
    MAC_FILE="mac/energy_polyethylene_${energy}keV.mac"
    DATA_FILE="results/data_energy_polyethylene_${energy}keV.root"
    
    # Verificar si ya existe el archivo de datos
    if [ -f "$DATA_FILE" ]; then
        echo "  Energía ${energy} keV: datos existentes (saltando)"
        continue
    fi
    
    # Determinar número de eventos basado en la energía
    # Energías bajas necesitan más eventos para mejor estadística
    if [ $energy -le 10 ]; then
        NEVENTS=1000000  # 1M eventos para energías ≤ 10 keV
    elif [ $energy -le 50 ]; then
        NEVENTS=500000   # 500k eventos para 10-50 keV  
    elif [ $energy -le 200 ]; then
        NEVENTS=200000   # 200k eventos para 50-200 keV
    else
        NEVENTS=100000   # 100k eventos para energías altas
    fi
    
    # Crear archivo macro
    echo "  Creando macro para energía ${energy} keV (${NEVENTS} eventos)..."
    cat > "$MAC_FILE" << EOF
# Configuración para polietileno - ${energy} keV
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial polyethylene
/detector/setThickness 5.0 cm

# Configurar energía del fotón
/gun/setEnergy ${energy} keV

# Inicializar
/run/initialize

# Ejecutar simulación (eventos optimizados por energía)
/run/beamOn ${NEVENTS}
EOF
    
    echo "  Simulando energía ${energy} keV (${NEVENTS} eventos)..."
    cd build
    ./gammaAtt "../$MAC_FILE"
    cd ..
    
    # GEANT4 genera el archivo como data_run_polyethylene.root, necesitamos renombrarlo
    TEMP_FILE="results/data_run_polyethylene.root"
    if [ -f "$TEMP_FILE" ]; then
        mv "$TEMP_FILE" "$DATA_FILE"
        echo "  Archivo renombrado: $TEMP_FILE -> $DATA_FILE"
    else
        echo "  ERROR: No se generó el archivo temporal $TEMP_FILE"
        exit 1
    fi
    
    # Verificar que se generó el archivo con el nombre correcto
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
root -l -b -q "analysis/multi_energy_analysis.C"

echo "Análisis ROOT completado"
echo ""

echo "Paso 3: Generando visualizaciones..."
echo "------------------------------------"

# Activar entorno Python y ejecutar visualización
echo "Activando entorno Python..."
source GA/bin/activate

echo "Generando gráficas..."
python3 analysis/plot_multi_energy.py

echo ""
echo "=========================================="
echo "  ANÁLISIS MULTI-ENERGÍA COMPLETADO"
echo "=========================================="
echo ""
echo "Datos generados en:"
echo "  results/multi_energy/"
echo ""
echo "Archivos generados:"
echo "  - energy_spectrum_comparison.csv"
echo "  - energy_spectrum_analysis.png"
echo "  - nist_style_comparison.png"
echo ""