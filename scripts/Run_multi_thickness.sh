#!/bin/bash

# Script Multi-Espesor Genérico
# Análisis de atenuación gamma vs espesor para un material dado.
# Ejecuta simulaciones GEANT4, análisis ROOT y visualización Python.
#
# Uso: ./scripts/Run_multi_thickness.sh <material>
# Ejemplo: ./scripts/Run_multi_thickness.sh water
#          ./scripts/Run_multi_thickness.sh bone

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
echo "  ANÁLISIS MULTI-ESPESOR: $MATERIAL_UPPER"
echo "========================================"
echo "Material: $MATERIAL"
echo "Análisis: Ley Beer-Lambert vs espesor"
echo "Rango: 0.5 - 15.0 cm"
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

# Verificar que estamos en el directorio raíz del proyecto
if [ ! -f "CMakeLists.txt" ] || [ ! -d "src" ]; then
    echo "ERROR: Este script debe ejecutarse desde el directorio raíz del proyecto."
    echo "Ejemplo de uso: ./scripts/Run_multi_thickness.sh water"
    exit 1
fi

# Verificar que el ejecutable de Geant4 exista
if [ ! -f "build/gammaAtt" ]; then
    echo "ERROR: Ejecutable GEANT4 no encontrado en build/gammaAtt."
    echo "Por favor, compila el proyecto primero: cd build && make"
    exit 1
fi

# --- 2. CONFIGURACIÓN DE LA SIMULACIÓN ---

# Crear directorio de resultados si no existe
RESULTS_DIR="results/multi_thickness"
mkdir -p "$RESULTS_DIR"

# Determinar el nombre del material en Geant4 y el nombre del archivo de salida temporal
case "$MATERIAL" in
  water)
    G4_MATERIAL="G4_WATER"
    TEMP_OUTPUT_FILE="results/data_run_G4_WATER.root"
    ;;
  bone)
    G4_MATERIAL="G4_BONE_COMPACT_ICRU"
    TEMP_OUTPUT_FILE="results/data_run_G4_BONE_COMPACT_ICRU.root"
    ;;
  muscle)
    G4_MATERIAL="G4_MUSCLE_SKELETAL_ICRP" # Este es el material que se envía a Geant4
    TEMP_OUTPUT_FILE="results/data_run_G4_MUSCLE_SKELETAL_ICRP.root" # Este debe ser el nombre que Geant4 genera
    ;;
  *)
    echo "ERROR: Material '$MATERIAL' no reconocido."
    echo "Materiales válidos: water, bone, muscle."
    exit 1
    ;;
esac

echo "Paso 1: Generando datos de simulación..."
echo "----------------------------------------"

# Array de espesores para simular
THICKNESS_VALUES=(0.5 1.0 2.0 3.0 5.0 7.5 10.0 15.0)

echo "Ejecutando simulaciones GEANT4 para ${#THICKNESS_VALUES[@]} espesores de $MATERIAL..."

for thickness in "${THICKNESS_VALUES[@]}"; do
    MAC_FILE="mac/thickness_${MATERIAL}_${thickness}.mac"
    DATA_FILE="${RESULTS_DIR}/data_thickness_${MATERIAL}_${thickness}.root"

    # Saltar si el archivo de resultados ya existe
    if [ -f "$DATA_FILE" ]; then
        echo "  Espesor ${thickness} cm: datos existentes (saltando)"
        continue
    fi

    # Crear archivo macro dinámicamente
    echo "  Creando macro para espesor ${thickness} cm..."
    cat > "$MAC_FILE" << EOF
# Configuración para ${MATERIAL} - ${thickness}cm
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial ${G4_MATERIAL}
/detector/setThickness ${thickness} cm

# Inicializar
/run/initialize

# Ejecutar simulación
/run/beamOn 100000
EOF

    echo "  Simulando espesor ${thickness} cm (100k eventos)..."
    ./build/gammaAtt "$MAC_FILE" > /dev/null 2>&1

    # Mover y renombrar el archivo de datos generado
    if [ -f "$TEMP_OUTPUT_FILE" ]; then
        mv "$TEMP_OUTPUT_FILE" "$DATA_FILE"
        echo "    Completado: $DATA_FILE"
    else
        echo "    WARNING: No se generó el archivo de datos '$TEMP_OUTPUT_FILE'"
    fi
done

echo "Simulaciones completadas."
echo ""

echo "Paso 2: Ejecutando análisis ROOT..."
echo "-----------------------------------"
root -l -b -q "analysis/Multi_thickness_analysis.C(\"${MATERIAL}\")"

echo ""
echo "Paso 3: Generando visualizaciones..."
echo "------------------------------------"
python3 analysis/Plot_multi_thickness.py "$MATERIAL"

echo ""
echo "=========================================="
echo "  ANÁLISIS MULTI-ESPESOR COMPLETADO"
echo "=========================================="
echo "Resultados generados en: ${RESULTS_DIR}/"