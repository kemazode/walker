cmake_minimum_required(VERSION 2.8)

project(Walker)
set(CMAKE_CXX_FLAGS "-lncursesw -lm -lpanelw -lmenuw -lyaml")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

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
            source/cmdline.c
            )
            
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(${PROJECT_NAME} ${SOURCES})
