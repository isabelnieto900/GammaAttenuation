cmake_minimum_required(VERSION 3.10)
project(GammaAttenuation)

# Generar compile_commands.json para VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Crear el directorio de resultados en la raíz del proyecto
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/results)

# Encontrar Geant4
find_package(Geant4 REQUIRED ui_all vis_all)

# Encontrar ROOT (opcional)
find_package(ROOT QUIET)
if(ROOT_FOUND)
    message(STATUS "ROOT encontrado en: ${ROOT_DIR}")
    message(STATUS "ROOT version: ${ROOT_VERSION}")
    include_directories(${ROOT_INCLUDE_DIRS})
    add_definitions(-DUSE_ROOT)
else()
    message(STATUS "ROOT no encontrado - compilando sin análisis ROOT")
endif()

include(${Geant4_USE_FILE})

# Archivos fuente
set(sources
    src/main.cc
    src/DetectorConstruction.cc
    src/PrimaryGeneratorAction.cc
    src/RunAction.cc
    src/EventAction.cc
    src/MiSensitiveDetector.cc
    src/MiHit.cc
    src/PhysicsListExtended.cc
    src/DetectorMessenger.cc
    src/PrimaryGeneratorMessenger.cc
    )

# Agregar la ruta de inclusión
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(gammaAtt ${sources})

# Enlazar librerías
target_link_libraries(gammaAtt ${Geant4_LIBRARIES})

# Si ROOT está disponible, enlazarlo también
if(ROOT_FOUND)
    target_link_libraries(gammaAtt ${ROOT_LIBRARIES})
endif()
