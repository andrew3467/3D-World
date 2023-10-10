cmake_minimum_required(VERSION 3.26)
project(World_Generator)

set(CMAKE_CXX_STANDARD 17)


find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)



set(IMGUI_SRC
        ${CMAKE_SOURCE_DIR}/external/ImGui/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/external/ImGui/imgui_impl_glfw.h
        ${CMAKE_SOURCE_DIR}/external/ImGui/imgui_impl_opengl3.cpp
        ${CMAKE_SOURCE_DIR}/external/ImGui/imgui_impl_opengl3.h
        ${CMAKE_SOURCE_DIR}/external/ImGui/imgui_impl_opengl3_loader.h
)

set(SIMPLEX_SRC
    ${CMAKE_SOURCE_DIR}/external/Simplex/SimplexNoise.h
    ${CMAKE_SOURCE_DIR}/external/Simplex/SimplexNoise.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/external/ImGui)

add_library(World_Generator
        ${IMGUI_SRC}
        ${SIMPLEX_SRC}
        Application.cpp
        Application.h
        TerrainChunk.cpp
        TerrainChunk.h
        MarchingTables.h
)


target_link_libraries(World_Generator
        PUBLIC
        glm::glm
        imgui::imgui
        yaml-cpp
)