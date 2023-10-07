//
// Created by apgra on 9/12/2023.
//

#include <iostream>
#include "Application.h"
#include "../Renderer/Renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <yaml.h>





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

        glEnable(GL_DEPTH_TEST);


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

        lit_shader = std::make_unique<Renderer::Shader>("../Engine/Shaders/solid_lit.glsl");
        unlit_shader = std::make_unique<Renderer::Shader>("../Engine/Shaders/solid_unlit.glsl");

        m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 4.0f), 4.0f);

        m_TerrainChunk = std::make_unique<TerrainChunk>(&m_TerrainConfig);

        loadConfig();
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        drawLights();


        glm::mat4 model = glm::mat4(1.0f);

        lit_shader->bind();

        lit_shader->setVec3("viewPos", m_Camera->position());

        lit_shader->setInt("numLights", m_Lights.size());
        for(int i = 0; i < m_Lights.size(); i++){
            std::string arrIndex = std::to_string(i);
            lit_shader->setPointLight(arrIndex, m_Lights[i]);
        }

        lit_shader->setVec3("baseColor", m_TerrainConfig.color);

        lit_shader->setMat4("model", model);
        lit_shader->setMat4("vp", m_Camera->viewProjection(m_Window->getSize()));

        Mesh::DrawMeshes(*lit_shader);
    }

    void Application::drawLights() {
        //Draw Point Lights
        unlit_shader->bind();
        glm::mat4 vp = m_Camera->viewProjection(m_Window->getSize());

        for(auto& light : m_Lights){
            glm::mat4 model = glm::translate(glm::mat4(1.0f), light.Position);
            model = glm::scale(model, glm::vec3(0.25f));
            unlit_shader->setMat4("mvp",  vp * model);
            unlit_shader->setVec3("color", light.Ambient);

            Renderer::Renderer::DrawCube(*unlit_shader);
        }
    }

    void Application::onImGUIRender() {
        if(!imGUIActive){
            return;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Config Window");

        bool updateMesh = false;
        if(ImGui::CollapsingHeader("Terrain Config")) {
            ImGui::Indent();

            //Save to file
            if(ImGui::Button("Save Config")){
                saveConfig();
            }

            int genType = m_TerrainConfig.genType;
            const char* genTypeNames[3] = {"Height Map", "Marching Cubes 3D"};
            const char* genTypeName = (genType >= 0 && genType < 2) ? genTypeNames[genType] : "Unknown";

            updateMesh |= ImGui::SliderInt("Generation Type", (int*)&m_TerrainConfig.genType, 0, 1, genTypeName);
            updateMesh |= ImGui::SliderInt("Seed", &m_TerrainConfig.seed, 0, 32767);
            updateMesh |= ImGui::SliderInt("Terrain Size", &m_TerrainConfig.size, 2, 16);
            updateMesh |= ImGui::SliderInt("Resolution", &m_TerrainConfig.resolution, 0, 5);

            if(ImGui::CollapsingHeader("Noise Config")){
                ImGui::Indent();
                updateMesh |= ImGui::SliderFloat2("Noise Scale", &m_TerrainConfig.noiseScale.x, 0, 1);
                updateMesh |= ImGui::SliderFloat3("Noise Offset", &m_TerrainConfig.noiseOffset.x, -10.0f, 10.0f);
                updateMesh |= ImGui::SliderInt("Octaves", &m_TerrainConfig.octaves, 1, 8);
                updateMesh |= ImGui::SliderFloat("Frequency", &m_TerrainConfig.frequency, 0.0f, 4.0f);
                updateMesh |= ImGui::SliderFloat("Amplitude", &m_TerrainConfig.amplitude, 0.0f, 1.0f);
                updateMesh |= ImGui::SliderFloat("Lacunarity", &m_TerrainConfig.lacunarity, 0.0f, 4.0f);
                updateMesh |= ImGui::SliderFloat("Persistence", &m_TerrainConfig.persistence, 0.0f, 4.0f);
            }


            if(m_TerrainConfig.genType == HeightMap && ImGui::CollapsingHeader("Height Map Config")) {
                ImGui::Indent();

                updateMesh |= ImGui::SliderFloat("Height Multiplier", &m_TerrainConfig.heightMultiplier, 0.1f, 8.0f);
            }

            if(m_TerrainConfig.genType == MarchingCube3D && ImGui::CollapsingHeader("Marching Cubes Config")) {
                ImGui::Indent();

                updateMesh |= ImGui::SliderInt("Height", &m_TerrainConfig.height, 1, 16);
                updateMesh |= ImGui::SliderFloat("Iso Surface", &m_TerrainConfig.isoLevel, 0.0f, 1.0f);
            }

            ImGui::Unindent();

            updateMesh |= ImGui::ColorPicker3("Ambient", &m_TerrainConfig.color.x);
        }
        if(ImGui::CollapsingHeader("Lighting Config")){
            ImGui::Indent();

            if(ImGui::Button("Create Point Light")){
               m_Lights.emplace_back();
            }

            //Individual Configs
            for(int i = 0; i < m_Lights.size(); i++){
                if(ImGui::CollapsingHeader(std::string("Light #").append(std::to_string(i)).c_str())){
                    ImGui::Indent();
                    ImGui::SliderFloat3("Position", &m_Lights[i].Position.x, -10.0f, 10.0f);
                    ImGui::SliderFloat("Constant", &m_Lights[i].Constant, 0.0f, 1.0f);
                    ImGui::SliderFloat("Linear", &m_Lights[i].Linear, 0.0f, 1.0f);
                    ImGui::SliderFloat("Exponential", &m_Lights[i].Exp, 0.0f, 1.0f);
                    ImGui::ColorPicker3("Ambient", &m_Lights[i].Ambient.x);
                    ImGui::ColorPicker3("Diffuse", &m_Lights[i].Diffuse.x);
                    ImGui::ColorPicker3("Specular", &m_Lights[i].Specular.x);
                }
            }
        }



        if(updateMesh){
            m_TerrainChunk->updateMesh();
        }


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Application::saveConfig() {
        yaml_parser_t parser;
        yaml_event_t event;
        int done = 0;

        yaml_parser_initialize(&parser);

        std::string input =  "...";
        size_t length = input.size();
        yaml_parser_set_input_string(&parser, input.c_str(), length);



        yaml_parser_delete(&parser);
    }

    void Application::loadConfig() {

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