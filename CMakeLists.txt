cmake_minimum_required(VERSION 2.8)

project(Walker)
set(CMAKE_CXX_FLAGS "-lncursesw -lm -lpanelw -lmenuw")
set(SOURCES main.cpp
            window.cpp
            scene.cpp
            object.cpp
            perlin.cpp
            )

add_executable(${PROJECT_NAME} ${SOURCES})
