//
// Created by apgra on 9/11/2023.
//

#include <memory>
#include <stdexcept>
#include "Window.h"


Window::Window(int width, int height, std::string title)
    : m_Width(width), m_Height(height), m_Title(std::move(title))
{
    if(!GlfwInitialized){
        initGLFW();
    }

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);

    glfwSwapInterval(0);

    if(!GladInitialized){
        initGLAD();
    }
}

Window::~Window() {

}

void Window::initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::initGLAD() {
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        throw std::runtime_error("ERROR: Failed to initialize GLAD");
    }
}

void Window::tick() const {
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(m_Window);
}

void Window::setUserPointer(void* ptr) {
    glfwSetWindowUserPointer(m_Window, ptr);
}

void Window::setKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(m_Window, callback);
}

void Window::setScrollCallback(GLFWscrollfun callback) {
    glfwSetScrollCallback(m_Window, callback);
}

void Window::setResizeCallback(GLFWwindowsizefun callback) {
    glfwSetWindowSizeCallback(m_Window, callback);
}
