cmake_minimum_required(VERSION 3.26)
project(Engine)

# Find dependencies
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(imgui CONFIG REQUIRED)


add_library(Engine
        Test.cpp
        )


target_link_libraries(
        Engine
        PRIVATE
        glfw
        glad::glad
        glm::glm
        imgui::imgui
        Freetype::Freetype
)

install(TARGETS Engine
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)