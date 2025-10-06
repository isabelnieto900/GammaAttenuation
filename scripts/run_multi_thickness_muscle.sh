#!/bin/bash

# Script Multi-Espesor
# Análisis de atenuación gamma vs espesor para músculo
# Ejecuta análisis ROOT + visualización Python

echo "========================================"
echo "  ANÁLISIS MULTI-ESPESOR"
echo "========================================"
echo "Material: Músculo Esquelético"
echo "Análisis: Ley Beer-Lambert vs espesor"
echo "Rango: 0.5 - 15.0 cm"
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

# Ir al directorio de análisis
cd "$(dirname "$0")" || exit 1
cd ../analysis || exit 1

echo "Paso 1: Generando datos de simulación..."
echo "----------------------------------------"

# Verificar que el ejecutable GEANT4 existe
if [ ! -f "../build/gammaAtt" ]; then
    echo "ERROR: Ejecutable GEANT4 no encontrado en ../build/gammaAtt"
    echo "Ejecuta: cd ../build && make"
    exit 1
fi

# Array de espesores para simular
THICKNESS_VALUES=(0.5 1.0 2.0 3.0 5.0 7.5 10.0 15.0)

echo "Ejecutando simulaciones GEANT4 para ${#THICKNESS_VALUES[@]} espesores..."

for thickness in "${THICKNESS_VALUES[@]}"; do
    MAC_FILE="../mac/thickness_muscle_${thickness}.mac"
    DATA_FILE="../results/data_thickness_muscle_${thickness}.root"
    
    # Verificar si ya existe el archivo de datos
    if [ -f "$DATA_FILE" ]; then
        echo "  Espesor ${thickness} cm: datos existentes (saltando)"
        continue
    fi
    
    # Verificar si existe el archivo macro
    if [ ! -f "$MAC_FILE" ]; then
        echo "  Creando macro para espesor ${thickness} cm..."
        cat > "$MAC_FILE" << EOF
# Configuración para músculo - ${thickness}cm
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial G4_MUSCLE_SKELETAL_ICRP
/detector/setThickness ${thickness} cm

# Inicializar
/run/initialize

# Ejecutar simulación
/run/beamOn 100000
EOF
    fi
    
    echo "  Simulando espesor ${thickness} cm (100k eventos)..."
    cd ../build
    ./gammaAtt "$MAC_FILE" > /dev/null 2>&1
    
    # Mover archivo de datos generado al directorio results
    if [ -f "../results/data_run_G4_MUSCLE_STRIATED_ICRU.root" ]; then
        mv "../results/data_run_G4_MUSCLE_STRIATED_ICRU.root" "$DATA_FILE"
        echo "    Completado: $DATA_FILE"
    else
        echo "    WARNING: No se generó archivo de datos"
    fi
    
    cd ../scripts
done

echo "Simulaciones completadas"
echo ""

echo "Paso 2: Ejecutando análisis ROOT..."
echo "-----------------------------------"

# Ejecutar análisis ROOT
cd /home/isabel/Physiscs_projects/GammaAtenuation
root -l -b -q "analysis/multi_thickness_muscle_analysis.C"

echo "Paso 3: Generando visualizaciones..."
echo "-----------------------------------"

# Activar entorno Python y ejecutar visualización
source GA/bin/activate
python3 analysis/plot_multi_thickness_muscle.py

echo ""
echo "=========================================="
echo "  ANÁLISIS MULTI-ESPESOR COMPLETADO"
echo "=========================================="
echo ""
echo "Datos generados en:"
echo "  /home/isabel/Physiscs_projects/GammaAtenuation/results/multi_thickness/"
echo ""
echo "Archivos generados:"
echo "  - thickness_muscle_analysis_data.csv"
echo "  - fit_muscle_results.txt"
echo "  - thickness_muscle_analysis_elegant.png"
echo "  - thickness_muscle_analysis_elegant.pdf"
echo "  - thickness_muscle_analysis_elegant.svg"
echo ""