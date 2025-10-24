# Analysis Scripts Documentation

Este directorio contiene los scripts de análisis para procesar los datos de simulación GEANT4.

## Scripts ROOT (Archivos .C)

### multi_thickness_analysis.C
**Propósito**: Analiza simulaciones con diferentes espesores de polietileno para verificar la ley de Beer-Lambert.

**Funcionalidad**:
- Procesa archivos ROOT de 8 espesores diferentes (0.5-15.0 cm)
- Calcula coeficientes de atenuación μ
- Realiza ajuste exponencial I = I₀e^(-μx)
- Genera archivo CSV con datos tabulados

**Uso**: Ejecutado automáticamente por `run_multi_thickness.sh`

### multi_material_analysis.C
**Propósito**: Compara atenuación entre diferentes materiales de blindaje a espesor fijo.

**Funcionalidad**:
- Analiza polietileno, concreto y plomo (5 cm cada uno)
- Calcula μ y μ/ρ para cada material
- Compara transmisiones y densidades
- Genera análisis de correlaciones para blindaje radiológico

**Uso**: Ejecutado automáticamente por `run_multi_material.sh`

### multi_energy_analysis.C
**Propósito**: Estudia dependencia energética de la atenuación.

**Funcionalidad**:
- Analiza diferentes energías de rayos gamma
- Calcula coeficientes de atenuación másicos
- Compara con datos NIST
- Estudia dependencia E^(-n)

**Uso**: Ejecutado automáticamente por `run_multi_energy.sh`

## Scripts Python (Archivos .py)

### plot_multi_thickness.py
**Propósito**: Genera visualizaciones del análisis de espesores.

**Funcionalidad**:
- Lee datos CSV del análisis ROOT
- Crea gráficas de transmisión vs espesor
- Genera ajuste Beer-Lambert con estadísticas
- Produce archivos PNG, PDF y SVG

**Requisitos**: matplotlib, numpy, pandas

### plot_multi_material.py
**Propósito**: Visualiza comparación entre materiales.

**Funcionalidad**:
- Gráficas de barras comparativas
- Correlación densidad-atenuación
- Tabla de resultados
- Análisis estadístico

**Requisitos**: matplotlib, numpy, pandas

### plot_multi_energy.py
**Propósito**: Grafica dependencia energética.

**Funcionalidad**:
- Coeficientes vs energía
- Comparación con literatura
- Ajustes de potencia
- Análisis log-log

**Requisitos**: matplotlib, numpy, pandas

## Estructura de Datos

### Archivos de Entrada
- `data_run*.root` - Archivos ROOT de GEANT4
- Cada archivo contiene tree con datos de eventos

### Archivos de Salida
- `*_analysis_data.csv` - Datos tabulados para cada análisis
- `*_results.txt` - Resúmenes estadísticos
- `*.png/pdf/svg` - Visualizaciones

## Flujo de Trabajo

1. **Simulación GEANT4** → Genera archivos `.root`
2. **Análisis ROOT** → Procesa datos y genera CSV
3. **Visualización Python** → Crea gráficas finales

## Configuración Python

Todos los scripts requieren el entorno virtual `GA`:

```bash
source GA/bin/activate
python analysis/plot_*.py
```

## Parámetros de Análisis

### Energía estándar: 662 keV (Cs-137)
### Materiales de blindaje estándar:
- Polietileno (HDPE): ρ = 0.95 g/cm³
- Concreto: ρ = 2.3 g/cm³  
- Plomo (Pb): ρ = 11.35 g/cm³

### Espesores estándar: 0.5, 1.0, 2.0, 3.0, 5.0, 7.5, 10.0, 15.0 cm

## Validación

Cada script incluye:
- Verificación de archivos de entrada
- Validación estadística
- Comparación con valores de literatura
- Cálculo de incertidumbres