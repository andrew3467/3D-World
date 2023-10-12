//
// Created by apgra on 9/18/2023.
//

#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H



#include "../Renderer/Shader.h"
#include "../Engine/Camera.h"
#include "../Engine/Window.h"
#include "TerrainChunk.h"
#include "../Engine/Lights.h"
#define glm_enable_experimental
#include "glm/gtx/hash.hpp"


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

        void saveConfig();
        void loadConfig();

        void drawLights();

        void updateActiveChunks();
        void updateFPS();

        void processInput(GLFWwindow* window);
    public:


        Camera* GetCamera() {return m_Camera.get();}
    private:
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        int frameCount = 0;

        std::unique_ptr<Camera> m_Camera;

        std::unique_ptr<Window> m_Window;
        TerrainConfig m_TerrainConfig;

        int m_ViewDistance = 4;

        std::unordered_map<glm::ivec2, std::unique_ptr<TerrainChunk>> m_TerrainChunks;
        std::vector<glm::ivec2> m_ActiveTerrainChunks;

        std::vector<PointLight> m_Lights;
        DirectionalLight dirLight;

    private:
        static void onKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    };
}


#endif //APPLICATION_APPLICATION_H
