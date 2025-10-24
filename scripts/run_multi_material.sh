#!/bin/bash

# Script Multi-Material
# Comparación de atenuación entre materiales de blindaje
# Ejecuta análisis ROOT + visualización Python

echo "========================================"
echo "  ANÁLISIS MULTI-MATERIAL"
echo "========================================"
echo "Materiales: Polietileno, Concreto, Plomo"
echo "Espesor: 5.0 cm fijo"
echo "Análisis: Comparación propiedades de blindaje"
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

# Crear directorio de resultados
mkdir -p /home/isabel/Physiscs_projects/GammaAtenuation/results/multi_material

# Ir al directorio principal
cd /home/isabel/Physiscs_projects/GammaAtenuation

echo "Paso 1: Generando datos de simulación multi-material..."
echo "------------------------------------------------------"

# Verificar que el ejecutable GEANT4 existe
if [ ! -f "/home/isabel/Physiscs_projects/GammaAtenuation/build/gammaAtt" ]; then
    echo "ERROR: Ejecutable GEANT4 no encontrado en /home/isabel/Physiscs_projects/GammaAtenuation/build/gammaAtt"
    echo "Ejecuta: cd build && make"
    exit 1
fi

# Materiales a simular (espesor fijo 5.0 cm)
declare -A MATERIALS
MATERIALS["polyethylene"]="polyethylene"
MATERIALS["concrete"]="concrete"
MATERIALS["lead"]="lead"

THICKNESS="5.0"

echo "Ejecutando simulaciones para ${#MATERIALS[@]} materiales (espesor ${THICKNESS} cm)..."

for material in "${!MATERIALS[@]}"; do
    MAC_FILE="/home/isabel/Physiscs_projects/GammaAtenuation/mac/material_${material}_${THICKNESS}cm.mac"
    DATA_FILE="../results/data_${material}_${THICKNESS}cm.root"
    
    # Verificar si ya existe el archivo de datos
    if [ -f "$DATA_FILE" ]; then
        echo "  Material ${material}: datos existentes (saltando)"
        continue
    fi
    
    # Crear archivo macro
    echo "  Creando macro para material ${material}..."
    cat > "$MAC_FILE" << EOF
# Configuración para ${material} - ${THICKNESS}cm
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0

# Configurar detector
/detector/setMaterial ${material}
/detector/setThickness ${THICKNESS} cm

# Inicializar
/run/initialize

# Ejecutar simulación
/run/beamOn 100000
EOF
    
    echo "  Simulando material ${material} (100k eventos)..."
    cd /home/isabel/Physiscs_projects/GammaAtenuation/build
    ./gammaAtt "$MAC_FILE"
    
    # Mover archivo de datos generado al directorio results
        if [ -f "/home/isabel/Physiscs_projects/GammaAtenuation/results/data_run_${material}.root" ]; then
            mv /home/isabel/Physiscs_projects/GammaAtenuation/results/data_run_${material}.root /home/isabel/Physiscs_projects/GammaAtenuation/results/material_${material}_5cm.root
            echo "Archivo movido: material_${material}_5cm.root"
        else
            echo "WARNING: No se generó archivo de datos para material ${material}"
        fi
    cd ../scripts
done

echo "Simulaciones multi-material completadas"
echo ""

echo "Paso 2: Ejecutando análisis ROOT..."
echo "-----------------------------------"

# Ejecutar análisis ROOT
cd /home/isabel/Physiscs_projects/GammaAtenuation
root -l -b -q "analysis/multi_material_analysis.C"

echo "Análisis ROOT completado"
echo ""

echo "=========================================="
echo "  ANÁLISIS MULTI-MATERIAL COMPLETADO"
echo "=========================================="
echo ""
echo "Datos generados en:"
echo "  /home/isabel/Physiscs_projects/GammaAtenuation/results/multi_material/"
echo ""
echo "Para generar las gráficas, ejecuta:"
echo "  source GA/bin/activate"
echo "  python analysis/plot_multi_material.py"
echo ""