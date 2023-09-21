//
// Created by apgra on 9/12/2023.
//

#include <iostream>
#include "Application.h"
#include "../Renderer/Renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace WorldGenerator {

    bool imGUIActive = false;
    bool wireFrameMode = false;

    void Application::Run() {
        onStart();

        while (!m_Window->shouldClose()) {
            onTick();
            onRender();
            onImGUIRender();
        }

        onClose();
    }

    void Application::onStart() {
        m_Window = std::make_unique<Window>(1280, 720, "3D World");
        m_Window->setUserPointer(this);

        m_Window->setKeyCallback(onKeyPressed);
        m_Window->setScrollCallback(onScroll);


        //Setup IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        const char* glsl_version = "#version 460";
        ImGui_ImplGlfw_InitForOpenGL(m_Window->getWindow(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        shader = std::make_unique<Renderer::Shader>("../Engine/Shaders/solid_unlit.glsl");

        m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 4.0f), 4.0f);

        m_TerrainChunk = std::make_unique<TerrainChunk>(&m_TerrainConfig);
    }

    void Application::onClose() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

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
        shader->setVec3("color", m_ChunkColor);

        shader->setMat4("mvp", model * m_Camera->viewProjection(m_Window->getSize()));

        Mesh::DrawMeshes(*shader);
    }

    void Application::onImGUIRender() {
        if(!imGUIActive){
            return;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Config Window");


        float frameDiff = 0;
        if(lastFrame > 1){
            frameDiff = glfwGetTime() - lastFrame;
        }
        std::string secPerFrame = std::string("Seconds per frame").append(std::to_string(1000 / frameDiff));
        ImGui::Text(secPerFrame.c_str());

        bool updateMesh = false;
        if(ImGui::CollapsingHeader("Terrain Config")) {
            ImGui::Indent();

            updateMesh |= ImGui::SliderInt("Terrain Size", &m_TerrainConfig.size, 2, 16);
            updateMesh |= ImGui::SliderInt("Resolution", &m_TerrainConfig.resolution, 0, 5);
            updateMesh |= ImGui::SliderFloat("Noise Scale", &m_TerrainConfig.noiseScale, 0, 10);
            updateMesh |= ImGui::SliderFloat2("Noise Offset", &m_TerrainConfig.noiseOffset.x, -10.0f, 10.0f);
            updateMesh |= ImGui::SliderFloat("Height", &m_TerrainConfig.height, 0.1f, 8.0f);
            updateMesh |= ImGui::SliderInt("Octaves", &m_TerrainConfig.octaves, 1, 8);

            ImGui::ColorPicker3("Color", &m_ChunkColor.x);
        }



        if(updateMesh){
            m_TerrainChunk->updateMesh();
        }


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

        //Mouse Input
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) {
            static double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            m_Camera->rotate((float) mouseX, (float) mouseY);
        }
    }

    void Application::onKeyPressed(GLFWwindow *window, int key, int scancode, int action, int mods) {
        Application* app = (Application*) glfwGetWindowUserPointer(window);

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
            imGUIActive = !imGUIActive;
        }

        if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
            wireFrameMode = !wireFrameMode;

            if(wireFrameMode){
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            }else{
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            }

        }
    }

    void Application::onScroll(GLFWwindow* window, double xoffset, double yoffset) {
        Application* app = (Application*) glfwGetWindowUserPointer(window);


        if(yoffset > 0){
            app->GetCamera()->increaseMoveSpeed(1.0f);
        }

        if(yoffset < 0){
            app->GetCamera()->increaseMoveSpeed(-1.0f);
        }
    }
}