cmake_minimum_required(VERSION 3.26)
project(Application)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Engine)
add_subdirectory(Renderer)
add_subdirectory(Generator)



set(SHADERS
    ${CMAKE_SOURCE_DIR}/Engine/Shaders/solid_unlit.glsl
)


add_executable(Application
        main.cpp
        ${SHADERS}
)

target_link_libraries(Application
        Renderer
        Engine
        World_Generator
)