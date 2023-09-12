//
// Created by apgra on 9/12/2023.
//

#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H


#include "Window.h"
#include "../Renderer/Shader.h"

namespace Engine {
    class Application {
    public:
        void Run();

    private:
        void onStart();
        void onTick();
        void onRender();
        void onClose();

        std::unique_ptr<Renderer::Shader> shader;

    private:
        std::unique_ptr<Window> m_Window;


        static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);

    };
}


#endif //APPLICATION_APPLICATION_H
