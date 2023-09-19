//
// Created by apgra on 9/18/2023.
//

#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H



#include "../Renderer/Shader.h"
#include "../Engine/Camera.h"
#include "../Engine/Window.h"
#include "TerrainChunk.h"

namespace WorldGenerator {
    class Application {
    public:
        void Run();

    private:
        void onStart();
        void onTick();
        void onRender();
        void onImGUIRender();
        void onClose();

        void processInput(GLFWwindow* window);

    private:
        glm::ivec2 meshSize = {4,4};

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        std::unique_ptr<Renderer::Shader> shader;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Window> m_Window;

        TerrainConfig m_TerrainConfig;
        std::unique_ptr<TerrainChunk> m_TerrainChunk;

    public:
        Camera* GetCamera() {return m_Camera.get();}

    private:

        static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}


#endif //APPLICATION_APPLICATION_H
