//
// Created by apgra on 9/12/2023.
//

#include <iostream>
#include "Application.h"
#include "../Renderer/Renderer.h"
#include "MeshGenerator.h"

namespace WorldGenerator {


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

        m_Mesh = std::make_unique<Mesh>(MeshGenerator::rectangleMesh(4, 4));
    }

    void Application::onClose() {
        glfwTerminate();
    }

    void Application::onTick() {
        processInput(m_Window->getWindow());

        m_Window->tick();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    void Application::onRender() {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);

        shader->bind();
        shader->setVec3("color", 1.0f, 1.0f, 0.0f);

        shader->setMat4("mvp", model * m_Camera->viewProjection(m_Window->getSize()));

        m_Mesh->draw(*shader);
        //Renderer::Renderer::DrawSquare(*shader);
    }

    void Application::onImGUIRender() {

    }

    void Application::processInput(GLFWwindow *window) {
        if(glfwGetKey(window, GLFW_KEY_W)){
            m_Camera->move(Forward, deltaTime);
        }
        if(glfwGetKey(window, GLFW_KEY_S)){
            m_Camera->move(Backward, deltaTime);
        }
        if(glfwGetKey(window, GLFW_KEY_A)){
            m_Camera->move(Left , deltaTime);
        }
        if(glfwGetKey(window, GLFW_KEY_D)){
            m_Camera->move(Right, deltaTime);
        }

        if(glfwGetKey(window, GLFW_KEY_UP)){
            m_Camera->increaseMoveSpeed(0.1f);
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN)){
            m_Camera->increaseMoveSpeed(-0.1f);
        }

        //Mouse Input
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) {
            static double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            m_Camera->rotate((float) mouseX, (float) mouseY);
        }
    }

    void Application::onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}