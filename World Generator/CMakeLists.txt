cmake_minimum_required(VERSION 3.26)
project(World_Generator)

set(CMAKE_CXX_STANDARD 17)


find_package(glm CONFIG REQUIRED)


add_library(World_Generator
        Application.cpp
        Application.h
        MeshGenerator.cpp
        MeshGenerator.h
)


target_link_libraries(World_Generator
        PUBLIC
        glm::glm
)
