cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(GammaAttenuation)

# Buscar Geant4
find_package(Geant4 REQUIRED ui_all vis_all)

# Incluir directorios
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Archivos fuente
set(SOURCES
    src/DetectorConstruction.cc
    src/PhysicsList.cc
    src/PrimaryGenerator.cc
    src/SensitiveDetector.cc
    main.cc
)

# Ejecutable
add_executable(GammaAttenuation ${SOURCES})

# Vincular librerías
target_link_libraries(GammaAttenuation ${Geant4_LIBRARIES})

# Instalar binarios y macros
install(TARGETS GammaAttenuation DESTINATION bin)
install(DIRECTORY macros DESTINATION share/GammaAttenuation)
