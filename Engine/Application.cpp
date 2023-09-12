//
// Created by apgra on 9/12/2023.
//

#include <iostream>
#include "Application.h"
#include "../Renderer/Renderer.h"



namespace Engine {


    void Application::Run() {
        onStart();

        while (!m_Window->shouldClose()) {
            onTick();
            onRender();
        }

        onClose();
    }

    void Application::onStart() {
        m_Window = std::make_unique<Window>(1280, 720, "3D World");
        m_Window->setUserPointer(this);

        m_Window->setKeyCallback(onKeyPressed);


        shader = std::make_unique<Renderer::Shader>("../Engine/Shaders/solid_unlit.glsl");
    }

    void Application::onClose() {
        glfwTerminate();
    }

    void Application::onTick() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        m_Window->tick();
    }

    void Application::onRender() {
        shader->bind();
        shader->setVec3("color", 0.1f, 0.5f, 0.8f);

        Renderer::Renderer::DrawSquare(*shader);
    }

    void Application::onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}