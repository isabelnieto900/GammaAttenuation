#!/bin/bash

# Script Multi-Energía Genérico
# Ejecuta simulaciones GEANT4 para un material y rango de energías.
# Analiza coeficientes de atenuación vs energía.
#
# Uso: ./scripts/Run_multi_energy.sh <material>
# Ejemplo: ./scripts/Run_multi_energy.sh water
#          ./scripts/Run_multi_energy.sh bone


# --- 1. VALIDACIÓN DE ENTRADA Y ENTORNO ---

MATERIAL=$1
if [ -z "$MATERIAL" ]; then
    echo "ERROR: Debes especificar un material como primer argumento."
    echo "Uso: $0 <material>"
    echo "Materiales disponibles: water, bone, muscle"
    exit 1
fi

MATERIAL_UPPER=$(echo "$MATERIAL" | tr '[:lower:]' '[:upper:]')

echo "========================================"
echo "  ANÁLISIS MULTI-ENERGÍA: $MATERIAL_UPPER"
echo "========================================"
echo "Material: $MATERIAL"
echo "Análisis: Coeficientes de atenuación vs energía"
echo "Rango: 1 keV - 20 MeV"
echo ""

# Verificar que los comandos necesarios estén disponibles
if ! command -v root &> /dev/null; then
    echo "ERROR: ROOT no está instalado o no está en el PATH."
    exit 1
fi
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python3 no está disponible."
    exit 1
fi

# Verificar que el ejecutable de Geant4 exista
if [ ! -f "build/gammaAtt" ]; then
    echo "ERROR: Ejecutable GEANT4 no encontrado en build/gammaAtt."
    echo "Asegúrate de estar en el directorio raíz del proyecto y de haber compilado con 'cd build && make'."
    exit 1
fi

# --- 2. CONFIGURACIÓN DE LA SIMULACIÓN ---

# Crear directorio de resultados específico para el material
RESULTS_DIR="results/${MATERIAL}/multi_energy"
mkdir -p "$RESULTS_DIR"

echo "Paso 1: Generando datos de simulación GEANT4..."
echo "----------------------------------------------"

# Energías a simular (en keV)
ENERGIES=(1 5 10 20 30 50 80 100 150 200 300 400 500 600 662 800 1000 1250 1500 2000 3000 4000 5000 6000 8000 10000 15000 20000)

echo "Ejecutando simulaciones para ${#ENERGIES[@]} energías en el material '$MATERIAL'..."

for energy in "${ENERGIES[@]}"; do
    MAC_FILE="mac/energy_${MATERIAL}_${energy}keV.mac"
    DATA_FILE="${RESULTS_DIR}/data_energy_${MATERIAL}_${energy}keV.root"
    
    # Saltar si el archivo de resultados ya existe
    if [ -f "$DATA_FILE" ]; then
        echo "  Energía ${energy} keV: datos existentes (saltando)"
        continue
    fi
    
    # Determinar número de eventos (NEVENTS) basado en la energía y el material
    # Se usan más eventos para bajas energías en agua para mejorar la estadística
    if [ "$MATERIAL" == "water" ] && [ $energy -le 10 ]; then
        NEVENTS=1000000
    elif [ "$MATERIAL" == "water" ] && [ $energy -le 50 ]; then
        NEVENTS=500000
    elif [ "$MATERIAL" == "water" ] && [ $energy -le 200 ]; then
        NEVENTS=200000
    else
        NEVENTS=100000
    fi
    
    # Crear archivo macro dinámicamente
    echo "  Creando macro para ${energy} keV (${NEVENTS} eventos)..."
    cat > "$MAC_FILE" << EOF
# Configuración para ${MATERIAL} - ${energy} keV
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial ${MATERIAL}
/detector/setThickness 5.0 cm

# Configurar energía del fotón
/gun/setEnergy ${energy} keV

# Inicializar
/run/initialize

# Ejecutar simulación
/run/beamOn ${NEVENTS}
EOF
    
    # Ejecutar la simulación
    echo "  Simulando ${energy} keV..."
    ./build/gammaAtt "$MAC_FILE" > /dev/null # Redirigir salida para no saturar la terminal
    
    # Renombrar el archivo de salida de Geant4
    # Asumimos que Geant4 crea 'results/data_run_<material>.root'
    TEMP_FILE="results/data_run_${MATERIAL}.root"
    if [ -f "$TEMP_FILE" ]; then
        mv "$TEMP_FILE" "$DATA_FILE"
    else
        echo "  ERROR: No se generó el archivo temporal $TEMP_FILE. Abortando."
        exit 1
    fi
    
    echo "  Energía ${energy} keV: ✓ completada"
done

echo ""
echo "Paso 2: Ejecutando análisis ROOT..."
echo "-----------------------------------"
root -l -b -q "analysis/Multi_energy_analysis.C(\"${MATERIAL}\")"

echo ""
echo "Paso 3: Generando visualizaciones..."
echo "------------------------------------"
python3 analysis/Plot_multi_energy.py "$MATERIAL"

echo ""
echo "=========================================="
echo "  ANÁLISIS PARA '$MATERIAL_UPPER' COMPLETADO"
echo "=========================================="
echo "Resultados generados en: $RESULTS_DIR/"