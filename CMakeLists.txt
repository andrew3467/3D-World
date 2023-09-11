cmake_minimum_required(VERSION 3.26)
project(Application)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Engine)
add_subdirectory(Renderer)

add_executable(Application
        main.cpp
        Renderer/Renderer.cpp
        Renderer/Renderer.h
)

target_link_libraries(Application
        Renderer
        Engine
)