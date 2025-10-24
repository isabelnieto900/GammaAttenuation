cmake_minimum_required(VERSION 3.10)
project(GammaAttenuation)

# Generar compile_commands.json para VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Crear el directorio de resultados en la raíz del proyecto
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/results)

# Encontrar Geant4
find_package(Geant4 REQUIRED ui_all vis_all multithreaded)

# Encontrar ROOT (obligatorio)
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
add_definitions(-DUSE_ROOT)
message(STATUS "ROOT encontrado (versión ${ROOT_VERSION}) - compilando con soporte ROOT.")

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
# El orden es importante: enlaza ROOT, luego Geant4 y sus dependencias como TBB.
target_link_libraries(gammaAtt ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} ${Geant4_TBB_LIBRARIES})
