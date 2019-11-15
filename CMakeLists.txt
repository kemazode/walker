cmake_minimum_required(VERSION 2.8)

project(Walker)
set(CMAKE_CXX_FLAGS "-lncursesw -lm -lpanelw -lmenuw -lyaml")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

set(GENERATE_GGO_INPUT source/opts.ggo)
set(GENERATE_GGO_OUTPUT opts)
message(STATUS "= = ${GENERATE_GGO_OUTPUT}")
find_program(GENGETOPT gengetopt)
if(NOT GENGETOPT)
    message(FATAL_ERROR "gengetopt not found\n")
  return()
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${GENERATE_GGO_OUTPUT}.c ${CMAKE_CURRENT_BINARY_DIR}/${GENERATE_GGO_OUTPUT}.h
    COMMAND ${GENGETOPT} -i ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATE_GGO_INPUT} -F ${GENERATE_GGO_OUTPUT} --output-dir=${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATE_GGO_INPUT}
)

set(SOURCES source/main.cpp
            source/window.cpp
            source/scene.cpp
            source/object.cpp
            source/perlin.cpp
            source/utils.cpp
            source/event.cpp
            source/map.cpp
            source/ui.cpp
            source/images.cpp
            ${GENERATE_GGO_OUTPUT}.c
            )
            
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(${PROJECT_NAME} ${SOURCES})
