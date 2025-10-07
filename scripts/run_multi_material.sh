#!/bin/bash

# Script Multi-Material
# Comparación de atenuación entre agua, músculo y hueso
# Ejecuta análisis ROOT + visualización Python

echo "========================================"
echo "  ANÁLISIS MULTI-MATERIAL"
echo "========================================"
echo "Materiales: Agua, Músculo, Hueso"
echo "Espesor: 5.0 cm fijo"
echo "Análisis: Comparación propiedades"
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

# Verificar que estamos en el directorio raíz del proyecto
if [ ! -d "build" ] || [ ! -f "CMakeLists.txt" ]; then
    echo "ERROR: Este script debe ejecutarse desde el directorio raíz del proyecto."
    echo "Ejemplo: ./scripts/run_multi_material.sh"
    exit 1
fi

# Crear directorio de resultados
RESULTS_DIR="results/multi_material"
mkdir -p "$RESULTS_DIR"

echo "Paso 1: Generando datos de simulación multi-material..."
echo "------------------------------------------------------"

# Verificar que el ejecutable GEANT4 existe
if [ ! -f "build/gammaAtt" ]; then
    echo "ERROR: Ejecutable GEANT4 no encontrado en build/gammaAtt."
    echo "Ejecuta: cd build && make"
    exit 1
fi

# Espesor fijo para la comparación
THICKNESS="5.0"

# Usamos un array asociativo para mapear nombres amigables a los nombres de Geant4
declare -A MATERIALS
MATERIALS=(
    [water]="G4_WATER"
    [muscle]="G4_MUSCLE_SKELETAL_ICRP"
    [bone]="G4_BONE_COMPACT_ICRU"
)

echo "Ejecutando simulaciones para ${#MATERIALS[@]} materiales (espesor ${THICKNESS} cm)..."

for material_name in "${!MATERIALS[@]}"; do
    g4_material="${MATERIALS[$material_name]}"
    
    MAC_FILE="mac/material_${material_name}_${THICKNESS}cm.mac"
    DATA_FILE="${RESULTS_DIR}/material_${material_name}_${THICKNESS}cm.root"
    
    # Verificar si ya existe el archivo de datos
    if [ -f "$DATA_FILE" ]; then
        echo "  Material ${material_name}: datos existentes (saltando)"
        continue
    fi
    
    # Crear archivo macro
    echo "  Creando macro para material ${material_name}..."
    cat > "$MAC_FILE" << EOF
# Configuración para ${material_name} - ${THICKNESS}cm
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial ${g4_material}
/detector/setThickness ${THICKNESS} cm

# Inicializar
/run/initialize

# Ejecutar simulación
/run/beamOn 100000
EOF
    
    echo "  Simulando material ${material_name} (100k eventos)..."
    ./build/gammaAtt "$MAC_FILE" > /dev/null 2>&1
    
    # Mover archivo de datos generado al directorio results
    # El nombre del archivo temporal depende del nombre del material en Geant4
    TEMP_FILE="results/data_run_${g4_material}.root"
    if [ -f "$TEMP_FILE" ]; then
        mv "$TEMP_FILE" "$DATA_FILE" 
        echo "    Completado: $DATA_FILE"
    else
        echo "    WARNING: No se generó archivo de datos para material ${material_name}"
    fi
done

echo "Simulaciones multi-material completadas"
echo ""

echo "Paso 2: Ejecutando análisis ROOT..."
echo "-----------------------------------"

# Ejecutar análisis ROOT
root -l -b -q "analysis/multi_material_analysis.C"

echo "Análisis ROOT completado"
echo ""

echo "=========================================="
echo "  ANÁLISIS MULTI-MATERIAL COMPLETADO"
echo "=========================================="
echo ""
echo "Datos generados en:"
echo "  ${RESULTS_DIR}/"
echo ""
echo "Para generar las gráficas, ejecuta:"
echo "  source GA/bin/activate"
echo "  python analysis/plot_multi_material.py"
echo ""