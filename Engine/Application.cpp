//
// Created by apgra on 9/12/2023.
//

#include <iostream>
#include "Application.h"


namespace Engine{
    void Application::Run(){
        onStart();

        while(!m_Window->shouldClose()){
            onTick();
            onRender();
        }

        onClose();
    }

    void Application::onStart() {
        m_Window = std::make_unique<Window>(1280, 720, "3D World");
        m_Window->setUserPointer(this);

        m_Window->setKeyCallback(onKeyPressed);
    }

    void Application::onClose() {
       glfwTerminate();
    }

    void Application::onTick() {


        m_Window->tick();
    }

    void Application::onRender() {

    }

    void Application::onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }
    }
}