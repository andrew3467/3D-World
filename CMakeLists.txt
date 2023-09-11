cmake_minimum_required(VERSION 3.26)
project(3D_World)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Engine)

add_executable(3D_World main.cpp
        Engine/Test.cpp)
