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

        m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 4.0f), 1.0f);
    }

    void Application::onClose() {
        glfwTerminate();
    }

    void Application::onTick() {


        m_Window->tick();
    }

    void Application::onRender() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);

        shader->bind();
        shader->setVec3("color", 1.0f, 1.0f, 0.0f);

        shader->setMat4("mvp", m_Camera->viewProjection(m_Window->getSize()) * model);

        Renderer::Renderer::DrawSquare(*shader);
    }

    void Application::onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        Application *app = (Application*) glfwGetWindowUserPointer(window);

        if(key == GLFW_KEY_W){
            app->GetCamera()->move(Forward, 1.0f);
        }
        if(key == GLFW_KEY_S){
            app->GetCamera()->move(Backward, 1.0f);
        }
        if(key == GLFW_KEY_A){
            app->GetCamera()->move(Left, 1.0f);
        }
        if(key == GLFW_KEY_D){
            app->GetCamera()->move(Right, 1.0f);
        }
    }
}