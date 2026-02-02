cmake_minimum_required(VERSION 3.16)
project(tarea_simulacion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Geant4 con UI y Visualización
find_package(Geant4 11.0 REQUIRED ui_all vis_all)

# Incluye cabeceras propias
include_directories(${PROJECT_SOURCE_DIR}/include)

# Fuentes
file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cc)

add_executable(tarea_simulacion ${SRC_FILES})
target_link_libraries(tarea_simulacion ${Geant4_LIBRARIES})

# Opcional: instalación
install(TARGETS tarea_simulacion DESTINATION bin)
