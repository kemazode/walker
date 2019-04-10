cmake_minimum_required(VERSION 2.8)

project(Walker)
set(CMAKE_CXX_FLAGS "-lncursesw -lm -lpanelw -lmenuw -lyaml")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES main.cpp
            window.cpp
            scene.cpp
            object.cpp
            perlin.cpp
            utils.cpp
            event.cpp
            map.cpp
            ui.cpp
            )
            
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(${PROJECT_NAME} ${SOURCES})
