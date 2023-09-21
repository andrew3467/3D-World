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

    public:
        Camera* GetCamera() {return m_Camera.get();}


    private:
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        std::unique_ptr<Renderer::Shader> shader;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Window> m_Window;

        TerrainConfig m_TerrainConfig;
        std::unique_ptr<TerrainChunk> m_TerrainChunk;

        glm::vec3 m_ChunkColor = {0.3294f, 0.7333f, 0.1921f};

    private:

        static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    };
}


#endif //APPLICATION_APPLICATION_H
