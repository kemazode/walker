cmake_minimum_required(VERSION 2.8)

project(Walker)
set(CMAKE_CXX_FLAGS "-lncursesw -lm -lpanelw -lmenuw -lyaml")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES sources/main.cpp
            sources/window.cpp
            sources/scene.cpp
            sources/object.cpp
            sources/perlin.cpp
            sources/utils.cpp
            sources/event.cpp
            sources/map.cpp
            sources/ui.cpp
            sources/images.cpp
            )
            
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(${PROJECT_NAME} ${SOURCES})
