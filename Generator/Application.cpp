//
// Created by apgra on 9/12/2023.
//

#include <iostream>
#include "Application.h"
#include "../Renderer/Renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <yaml-cpp/yaml.h>






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

        Shader::initializeShaders();


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

        m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 4.0f), 4.0f);

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


        Shaders::solid_lit->bind();

        Shaders::solid_lit->setVec3("viewPos", m_Camera->position());

        Shaders::solid_lit->setVec3("dirLight.direction", dirLight.Direction);
        Shaders::solid_lit->setVec3("dirLight.ambient", dirLight.Ambient);
        Shaders::solid_lit->setVec3("dirLight.diffuse", dirLight.Diffuse);
        Shaders::solid_lit->setVec3("dirLight.specular", dirLight.Specular);

        Shaders::solid_lit->setInt("numLights", m_Lights.size());
        for(int i = 0; i < m_Lights.size(); i++){
            std::string arrIndex = std::to_string(i);
            Shaders::solid_lit->setPointLight(arrIndex, m_Lights[i]);
        }

        Shaders::solid_lit->setVec3("baseColor", m_TerrainConfig.color);

        Shaders::solid_lit->setMat4("vp", m_Camera->viewProjection(m_Window->getSize()));
        
        for(auto& chunkID : m_ActiveTerrainChunks){
            
        }
    }

    void Application::updateActiveChunks() {
        glm::vec3 viewerPos = m_Camera->position();
    }

    void Application::drawLights() {
        //draw Point Lights
        Shaders::solid_unlit->bind();
        glm::mat4 vp = m_Camera->viewProjection(m_Window->getSize());

        for(auto& light : m_Lights){
            glm::mat4 model = glm::translate(glm::mat4(1.0f), light.Position);
            model = glm::scale(model, glm::vec3(0.25f));
            Shaders::solid_unlit->setMat4("mvp",  vp * model);
            Shaders::solid_unlit->setVec3("color", light.Ambient);

            Renderer::Renderer::DrawCube(*Shaders::solid_unlit);
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

        ImGui::SliderInt("View Distance", &m_ViewDistance, 1, 8);

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
            updateMesh |= ImGui::SliderInt("Terrain Size", &m_TerrainConfig.size, 2, 64);
            updateMesh |= ImGui::SliderInt("Resolution", &m_TerrainConfig.resolution, 0, 5);

            if(ImGui::CollapsingHeader("Noise Config")){
                ImGui::Indent();
                updateMesh |= ImGui::SliderFloat2("Noise Scale", &m_TerrainConfig.noiseScale.x, 0, 1);
                updateMesh |= ImGui::SliderFloat3("Noise Offset", &m_TerrainConfig.noiseOffset.x, -10.0f, 10.0f);
                updateMesh |= ImGui::SliderInt("Octaves", &m_TerrainConfig.octaves, 1, 8);
                updateMesh |= ImGui::SliderFloat("Frequency", &m_TerrainConfig.frequency, 0.0f, 4.0f);
                updateMesh |= ImGui::SliderFloat("Amplitude", &m_TerrainConfig.amplitude, 1.0f, 10.0f);
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
        if(ImGui::CollapsingHeader("Lighting Config")) {
            ImGui::Indent();

            if (ImGui::Button("Create Point Light")) {
                m_Lights.emplace_back();
            }

            if (ImGui::CollapsingHeader("Directional Light")) {
                ImGui::SliderFloat3("Direction", &dirLight.Direction.x, -180.0f, 180.0f);
                ImGui::ColorPicker3("Ambient", &dirLight.Ambient.x);
                ImGui::ColorPicker3("Diffuse", &dirLight.Diffuse.x);
                ImGui::ColorPicker3("Specular", &dirLight.Specular.x);
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
            for(auto& c : m_TerrainChunks){
                c.second.updateMesh();
            }
        }


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Application::saveConfig() {
        YAML::Node config;


        config["Terrain Settings"]["Seed"] = m_TerrainConfig.seed;
        config["Terrain Settings"]["Size"] = m_TerrainConfig.size;
        config["Terrain Settings"]["Height"] = m_TerrainConfig.height;
        config["Terrain Settings"]["Resolution"] = m_TerrainConfig.resolution;
        config["Terrain Settings"]["ISO Level"] = m_TerrainConfig.isoLevel;

        config["Terrain Settings"]["Color"].push_back(m_TerrainConfig.color.x);
        config["Terrain Settings"]["Color"].push_back(m_TerrainConfig.color.y);
        config["Terrain Settings"]["Color"].push_back(m_TerrainConfig.color.z);

        config["Noise Settings"]["Offsets"].push_back(m_TerrainConfig.noiseOffset.x);
        config["Noise Settings"]["Offsets"].push_back(m_TerrainConfig.noiseOffset.y);
        config["Noise Settings"]["Offsets"].push_back(m_TerrainConfig.noiseOffset.z);

        config["Noise Settings"]["Noise Scale"].push_back(m_TerrainConfig.noiseScale.x);
        config["Noise Settings"]["Noise Scale"].push_back(m_TerrainConfig.noiseScale.y);

        config["Terrain Settings"]["Height Multiplier"] = m_TerrainConfig.heightMultiplier;

        config["Noise Settings"]["Frequency"] = m_TerrainConfig.frequency;
        config["Noise Settings"]["Amplitude"] = m_TerrainConfig.amplitude;
        config["Noise Settings"]["Lacunarity"] = m_TerrainConfig.lacunarity;
        config["Noise Settings"]["Persistence"] = m_TerrainConfig.persistence;

        config["Noise Settings"]["Octaves"] = m_TerrainConfig.octaves;


        std::ofstream fout("config.yaml");
        fout << config;
    }
    void Application::loadConfig() {
        YAML::Node config = YAML::LoadFile("config.yaml");


        m_TerrainConfig.seed = config["Terrain Settings"]["Seed"].as<int>();
        m_TerrainConfig.size = config["Terrain Settings"]["Size"].as<int>();
        m_TerrainConfig.height = config["Terrain Settings"]["Height"].as<int>();
        m_TerrainConfig.resolution = config["Terrain Settings"]["Resolution"].as<int>();
        m_TerrainConfig.isoLevel = config["Terrain Settings"]["ISO Level"].as<float>();


        m_TerrainConfig.color.x = config["Terrain Settings"]["Color"][0].as<float>();
        m_TerrainConfig.color.y = config["Terrain Settings"]["Color"][1].as<float>();
        m_TerrainConfig.color.z = config["Terrain Settings"]["Color"][2].as<float>();


        m_TerrainConfig.noiseOffset.x = config["Noise Settings"]["Offsets"][0].as<float>();
        m_TerrainConfig.noiseOffset.y = config["Noise Settings"]["Offsets"][1].as<float>();
        m_TerrainConfig.noiseOffset.z = config["Noise Settings"]["Offsets"][2].as<float>();

        m_TerrainConfig.noiseScale.x = config["Noise Settings"]["Noise Scale"][0].as<float>();
        m_TerrainConfig.noiseScale.y = config["Noise Settings"]["Noise Scale"][1].as<float>();

        m_TerrainConfig.heightMultiplier = config["Terrain Settings"]["Height Multiplier"].as<float>();

        m_TerrainConfig.frequency = config["Noise Settings"]["Frequency"].as<float>();
        m_TerrainConfig.amplitude = config["Noise Settings"]["Amplitude"].as<float>();
        m_TerrainConfig.lacunarity = config["Noise Settings"]["Lacunarity"].as<float>();
        m_TerrainConfig.persistence = config["Noise Settings"]["Persistence"].as<float>();

        m_TerrainConfig.octaves = config["Noise Settings"]["Octaves"].as<int>();
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