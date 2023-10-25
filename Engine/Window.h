//
// Created by apgra on 9/11/2023.
//

#ifndef APPLICATION_WINDOW_H
#define APPLICATION_WINDOW_H


#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

class Window {
    struct DestroyglfwWin{

        void operator()(GLFWwindow* ptr){
            glfwDestroyWindow(ptr);
        }

    };

private:
    bool GlfwInitialized = false;
    bool GladInitialized = false;

    unsigned int m_ID;

    int m_Width, m_Height;
    std::string m_Title;

    GLFWwindow* m_Window;


private:
    void initGLFW();
    void initGLAD();

public:
    Window(int width, int height, std::string title);
    ~Window();

    Window(Window& other) = delete;

    void tick() const;
    bool shouldClose();

    glm::vec2 getSize() {return {m_Width, m_Height};}
    GLFWwindow* getGLFWWindow() {return m_Window;}

    void setUserPointer(void* ptr);
    void setKeyCallback(GLFWkeyfun callback);
    void setScrollCallback(GLFWscrollfun callback);
    void setResizeCallback(GLFWwindowsizefun callback);
};


#endif //APPLICATION_WINDOW_H
