cmake_minimum_required(VERSION 3.26)
project(Application)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Engine)
add_subdirectory(Renderer)
add_subdirectory("World Generator")

add_executable(Application
        main.cpp
)

target_link_libraries(Application
        Renderer
        Engine
        World_Generator
)