cmake_minimum_required(VERSION 3.26)
project(Renderer)

set(CMAKE_CXX_STANDARD 17)

find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(Renderer
        Renderer.cpp
        Renderer.h
        Shader.cpp
        Shader.h
)

target_link_libraries(Renderer
        PUBLIC
        glad::glad
        glm::glm
)