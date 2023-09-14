//
// Created by apgra on 9/12/2023.
//

#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H


#include "Window.h"
#include "../Renderer/Shader.h"
#include "Camera.h"

namespace Engine {
    class Application {
    public:
        void Run();

    private:
        void onStart();
        void onTick();
        void onRender();
        void onClose();

        void processInput(GLFWwindow* window);

    private:
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        std::unique_ptr<Renderer::Shader> shader;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Window> m_Window;

    public:
        Camera* GetCamera() {return m_Camera.get();}

    private:

        static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}


#endif //APPLICATION_APPLICATION_H
