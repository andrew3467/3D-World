cmake_minimum_required(VERSION 3.26)
project(Engine)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)


add_library(Engine
        Window.cpp
        Window.h
        Camera.cpp
        Camera.h
        Mesh.cpp
        Mesh.h
        Helper/Singleton.h
        Lights.h
        MeshRenderer.cpp
        MeshRenderer.h
        Transform.h
        Transform.cpp
)


target_link_libraries(Engine
        PUBLIC
        glad::glad
        glfw
        glm::glm
)
