# Gamma Attenuation Simulation 

Este proyecto simula la atenuación de rayos gamma usando GEANT4 para estudiar la interacción de radiación gamma con diferentes materiales y espesores.

## Requisitos

- GEANT4 v11.3.2 o superior
- ROOT v6.36.04 o superior
- CMake v3.16 o superior
- Python 3.12 con matplotlib, numpy, pandas

## Compilación

```bash
mkdir build
cd build
cmake ..
make
```

## Estructura del Proyecto

```
GammaAtenuation/
├── src/                          # Código fuente GEANT4
├── include/                      # Headers GEANT4
├── mac/                          # Archivos macro para simulaciones
├── analysis/                     # Scripts de análisis
│   ├── multi_thickness_analysis.C   # Análisis ley Beer-Lambert
│   ├── multi_material_analysis.C    # (Aún no refactorizado)
│   ├── Multi_energy_analysis.C      # Script genérico para análisis multi-energía
│   ├── plot_multi_thickness.py      # Gráficas espesor vs atenuación
│   ├── plot_multi_material.py       # (Aún no refactorizado)
│   └── Plot_multi_energy.py         # Script genérico para gráficas multi-energía
├── scripts/                      # Scripts de automatización
│   ├── run_multi_thickness.sh       # (Aún no refactorizado)
│   ├── run_multi_material.sh        # Ejecuta análisis de materiales
│   ├── Run_multi_energy.sh          # Ejecuta análisis multi-energía
│   └── run_complete_analysis.sh     # Ejecuta análisis completo
├── results/                      # Resultados de simulaciones
├── build/                        # Directorio de compilación
└── GA/                          # Entorno virtual Python
```

## Scripts de Análisis

### run_multi_thickness.sh
Ejecuta simulaciones con diferentes espesores de agua (0.5-15.0 cm) para verificar la ley de Beer-Lambert.

### run_multi_material.sh  
Compara atenuación entre agua, músculo esquelético y hueso compacto a espesor fijo de 5 cm.

### Run_multi_energy.sh <material>
Script genérico que analiza la atenuación para un material específico (`water`, `bone`, `muscle`) en un rango de energías. Estudia la dependencia energética y compara los resultados con la base de datos del NIST.

### run_complete_analysis.sh
Ejecuta los tres análisis anteriores secuencialmente.

## Uso

1. **Compilar el proyecto:**
```bash
mkdir build && cd build && cmake .. && make
```

2. **Configurar entorno Python:**
```bash
python3 -m venv GA
source GA/bin/activate
pip install matplotlib numpy pandas
```

3. **Ejecutar análisis específico:**
```bash
./scripts/run_multi_thickness.sh    # Análisis espesores
./scripts/run_multi_material.sh <material>    # Análisis materiales  
./scripts/run_multi_energy.sh       # Análisis energías
```

4. **Ejecutar análisis completo:**
```bash
./scripts/run_complete_analysis.sh
```

## Resultados

Los resultados se generan en `results/` con subdirectorios por tipo de análisis:
- `multi_thickness/` - Datos y gráficas ley Beer-Lambert
- `multi_material/` - Comparación de materiales
- `multi_energy/` - Dependencia energética

Cada análisis genera archivos CSV con datos numéricos y visualizaciones en PNG/PDF/SVG.