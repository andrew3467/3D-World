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
#include <thread>
#include <imgui_stdlib.h>


namespace WorldGenerator {
    bool imGUIActive = false;
    bool wireFrameMode = false;
    bool infiniteTerrain = false;

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
        m_Window->setResizeCallback(onWindowResize);

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
        ImGui_ImplGlfw_InitForOpenGL(m_Window->getGLFWWindow(), true);
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
        processInput(m_Window->getGLFWWindow());

        //Only call if viewer moves
        if(m_Camera->position() != m_PrevViewerPos){
            updateActiveChunks();


            m_PrevViewerPos = m_Camera->position();
        }


        m_Window->tick();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateFPS();
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

        Shaders::solid_lit->setInt("numBiomes", m_TerrainConfig.biomes.size());
        for(int i = 0; i < m_TerrainConfig.biomes.size(); i++){
            std::string arrIndex = std::to_string(i);
            Shaders::solid_lit->setFloat("biomes[" + arrIndex + "].height", m_TerrainConfig.biomes[i].height);
            Shaders::solid_lit->setVec3("biomes[" + arrIndex + "].color", m_TerrainConfig.biomes[i].color);
        }

        Shaders::solid_lit->setInt("exp", m_NoiseConfig.exp);
        Shaders::solid_lit->setFloat("heightMultiplier", m_NoiseConfig.heightMultiplier);

        Shaders::solid_lit->setMat4("projection", m_Camera->projection(m_Window->getSize()));
        Shaders::solid_lit->setMat4("view", m_Camera->view());

        if(infiniteTerrain){
            for (auto &chunkID: m_ActiveTerrainChunks) {
                m_TerrainChunks.at(chunkID)->draw();
            }
        }else{
            m_TerrainChunks.at({0,0})->draw();
        }
    }

    void Application::updateActiveChunks() {
        if(!infiniteTerrain) {
            for(int z = 0; z < m_TestTerrainSize.y; z++){
                for(int x = 0; x < m_TestTerrainSize.x; x++){
                    glm::ivec2 chunkCoord(x,z);

                    if(m_TerrainChunks.find(chunkCoord) == m_TerrainChunks.end()){
                        createNewChunk(chunkCoord);
                    }

                    m_ActiveTerrainChunks.push_back(chunkCoord);
                }
            }
            return;
        }

        glm::vec3 viewerPos = m_Camera->position();
        glm::ivec2 viewerChunkCoord = {viewerPos.x / m_TerrainConfig.size,
                                      viewerPos.z / m_TerrainConfig.size};


        m_ActiveTerrainChunks.clear();
        for(int z = -m_ViewDistance; z < m_ViewDistance; z++){
            for(int x = -m_ViewDistance; x < m_ViewDistance; x++) {
                glm::ivec2 chunkCoord = glm::ivec2(x, z) + viewerChunkCoord;

                if (m_TerrainChunks.find(chunkCoord) == m_TerrainChunks.end()) {
                    createNewChunk(chunkCoord);
                }

                m_ActiveTerrainChunks.push_back(chunkCoord);
            }
        }
    }

    void Application::createNewChunk(glm::ivec2 chunkCoord) {
        m_TerrainChunks[chunkCoord] = std::make_unique<TerrainChunk>(glm::vec3(chunkCoord.x, 0.0f, chunkCoord.y), &m_TerrainConfig, &m_NoiseConfig, &m_ErosionConfig);
    }

    void Application::updateFPS() {
        static double lastFrame = 0;
        // Measure speed
        double currentTime = glfwGetTime();
        double delta = currentTime - lastFrame;
        frameCount++;
        if ( delta >= 1.0 ){ // If last cout was more than 1 sec ago

            int fps = double(frameCount) / delta;

            std::stringstream ss;
            ss << "3D World" << " [" << fps << " FPS]";

            glfwSetWindowTitle(m_Window->getGLFWWindow(), ss.str().c_str());

            frameCount = 0;
            lastFrame = currentTime;
        }
    }

    void Application::drawLights() {
        //draw Point Lights
        Shaders::solid_unlit->bind();
        glm::mat4 vp = m_Camera->viewProjection(m_Window->getSize());

        Shaders::solid_unlit->setMat4("projection", m_Camera->projection(m_Window->getSize()));
        Shaders::solid_unlit->setMat4("view", m_Camera->view());

        for(auto& light : m_Lights){
            glm::mat4 model = glm::translate(glm::mat4(1.0f), light.Position);
            model = glm::scale(model, glm::vec3(0.25f));
            Shaders::solid_unlit->setMat4("model",  model);
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


        ImGui::Checkbox("Infinite Terrain", &infiniteTerrain);
        ImGui::SliderInt("View Distance", &m_ViewDistance, 1, 8);

        if(!infiniteTerrain){
            if(ImGui::SliderInt2("Test Size", &m_TestTerrainSize.x, 1, 4)){
                updateActiveChunks();
            }
        }

        //Save to file
        if(ImGui::Button("Save Config")){
            saveConfig();
        }

        bool updateMesh = false;
        bool erosionSim = false;
        if(ImGui::Button("Run Erosion Sim")){
            updateMesh = true;
            erosionSim = true;
        }

        if(ImGui::CollapsingHeader("Terrain Config")) {
            ImGui::Indent();

            int genType = m_TerrainConfig.genType;
            const char* genTypeNames[3] = {"Height Map", "Height Map fBm", "Noiseless"};
            const char* genTypeName = (genType >= 0 && genType < 3) ? genTypeNames[genType] : "Unknown";

            updateMesh |= ImGui::SliderInt("Generation Type", (int*)&m_TerrainConfig.genType, 0, 2, genTypeName);
            updateMesh |= ImGui::SliderInt("Seed", &m_TerrainConfig.seed, 0, 32767);
            updateMesh |= ImGui::SliderInt("Terrain Size", &m_TerrainConfig.size, 2, 64);
            updateMesh |= ImGui::SliderInt("Resolution", &m_TerrainConfig.resolution, 0, 5);

            if(ImGui::CollapsingHeader("Noise Config")){
                ImGui::Indent();
                updateMesh |= ImGui::SliderFloat2("Noise Scale", &m_NoiseConfig.noiseScale.x, 0, 1);
                updateMesh |= ImGui::SliderFloat3("Noise Offset", &m_NoiseConfig.noiseOffset.x, -10.0f, 10.0f);
                updateMesh |= ImGui::SliderInt("Octaves", &m_NoiseConfig.octaves, 1, 10);

                //Don't need to update mesh, changes are made GPU side
                ImGui::SliderInt("EXP", &m_NoiseConfig.exp, 0, 12);
                ImGui::SliderFloat("Height Multiplier", &m_NoiseConfig.heightMultiplier, 0.0f, 20.0f);

                updateMesh |= ImGui::SliderFloat("Frequency", &m_NoiseConfig.frequency, 0.0f, 8.0f);
                updateMesh |= ImGui::SliderFloat("Lacunarity", &m_NoiseConfig.lacunarity, 0.0f, 1.0f);
                updateMesh |= ImGui::SliderFloat("Persistence", &m_NoiseConfig.persistence, 0.0f, 8.0f);
            }
            if(ImGui::CollapsingHeader("Biome Config")){
                if(ImGui::Button("Create Biome")){
                    m_TerrainConfig.biomes.emplace_back();
                    m_TerrainConfig.biomes[m_TerrainConfig.biomes.size()-1].name = std::string("Biome: " + std::to_string(m_TerrainConfig.biomes.size() + 1));
                }
                for(int i = 0; i < m_TerrainConfig.biomes.size(); i++){
                    if(ImGui::CollapsingHeader(m_TerrainConfig.biomes[i].name.c_str())){
                        if(ImGui::Button("Delete")){
                            m_TerrainConfig.biomes.erase(m_TerrainConfig.biomes.begin() + i);
                        }

                        ImGui::InputText("Name", &m_TerrainConfig.biomes[i].name);
                        ImGui::SliderFloat("Height", &m_TerrainConfig.biomes[i].height, 0.0f, 1.0f);
                        ImGui::ColorPicker3("Color", &m_TerrainConfig.biomes[i].color.x);
                    }
                }
            }


            if(m_TerrainConfig.genType == HeightMap && ImGui::CollapsingHeader("Height Map Config")) {
                ImGui::Indent();
            }

            ImGui::Unindent();
        }
        if(ImGui::CollapsingHeader("Erosion Config")){
            ImGui::InputInt("Num Droplets", &m_ErosionConfig.numDroplets);
            ImGui::InputInt("Max Steps", &m_ErosionConfig.maxSteps);

            ImGui::SliderFloat("Inertia", &m_ErosionConfig.inertia, 0.0f, 1.0f);
            ImGui::SliderFloat("Erosion Rate", &m_ErosionConfig.erosionRate, 0.01f, 1.0f);
            ImGui::SliderFloat("Deposition Rate", &m_ErosionConfig.depositionRate, 0.01f, 1.0f);
            ImGui::SliderFloat("Evaporation Rate", &m_ErosionConfig.evaporationRate, 0.01f, 1.0f);
            ImGui::SliderFloat("Min Slope-", &m_ErosionConfig.minSlope, 0.01f, 0.95f);

            ImGui::Spacing();
            ImGui::SliderInt("Particle Capacity", &m_ErosionConfig.particleCapacity, 0, 32);
            ImGui::SliderInt("Particle Radius", &m_ErosionConfig.particleRadius, 1, 10);

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
            for(auto& entry : m_TerrainChunks){
                auto& chunk = entry.second;
                chunk->updateMesh(erosionSim);
            }
        }


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    void Application::saveConfig() {
        YAML::Node config;

        config["Viewer Settings"]["View Distance"] = m_ViewDistance;
        config["Viewer Settings"]["Testing Size"][0] = m_TestTerrainSize.x;
        config["Viewer Settings"]["Testing Size"][1] = m_TestTerrainSize.y;


        config["Terrain Settings"]["Seed"] = m_TerrainConfig.seed;
        config["Terrain Settings"]["Size"] = m_TerrainConfig.size;
        config["Terrain Settings"]["Height"] = m_TerrainConfig.height;
        config["Terrain Settings"]["Resolution"] = m_TerrainConfig.resolution;
        config["Terrain Settings"]["ISO Level"] = m_TerrainConfig.isoLevel;


        config["Terrain Settings"]["Biomes"]["Count"] = m_TerrainConfig.biomes.size();
        for(int i = 0; i < m_TerrainConfig.biomes.size(); i++){
            config["Terrain Settings"]["Biomes"][i]["Name"] = m_TerrainConfig.biomes[i].name;
            config["Terrain Settings"]["Biomes"][i]["Height"] = m_TerrainConfig.biomes[i].height;

            config["Terrain Settings"]["Biomes"][i]["Color"].push_back(m_TerrainConfig.biomes[i].color.x);
            config["Terrain Settings"]["Biomes"][i]["Color"].push_back(m_TerrainConfig.biomes[i].color.y);
            config["Terrain Settings"]["Biomes"][i]["Color"].push_back(m_TerrainConfig.biomes[i].color.z);
        }


        config["Noise Settings"]["Offsets"].push_back(m_NoiseConfig.noiseOffset.x);
        config["Noise Settings"]["Offsets"].push_back(m_NoiseConfig.noiseOffset.y);
        config["Noise Settings"]["Offsets"].push_back(m_NoiseConfig.noiseOffset.z);

        config["Noise Settings"]["Noise Scale"].push_back(m_NoiseConfig.noiseScale.x);
        config["Noise Settings"]["Noise Scale"].push_back(m_NoiseConfig.noiseScale.y);

        config["Noise Settings"]["EXP"] = m_NoiseConfig.exp;
        config["Noise Settings"]["Height Multiplier"] = m_NoiseConfig.heightMultiplier;

        config["Noise Settings"]["Frequency"] = m_NoiseConfig.frequency;
        config["Noise Settings"]["Lacunarity"] = m_NoiseConfig.lacunarity;
        config["Noise Settings"]["Persistence"] = m_NoiseConfig.persistence;

        config["Noise Settings"]["Octaves"] = m_NoiseConfig.octaves;
        
        
        //LIGHTS
        config["Light Settings"]["Directional Light"]["Direction"].push_back(dirLight.Direction.x);
        config["Light Settings"]["Directional Light"]["Direction"].push_back(dirLight.Direction.y);
        config["Light Settings"]["Directional Light"]["Direction"].push_back(dirLight.Direction.z);
        
        config["Light Settings"]["Directional Light"]["Ambient"].push_back(dirLight.Ambient.x);
        config["Light Settings"]["Directional Light"]["Ambient"].push_back(dirLight.Ambient.y);
        config["Light Settings"]["Directional Light"]["Ambient"].push_back(dirLight.Ambient.z);

        config["Light Settings"]["Directional Light"]["Diffuse"].push_back(dirLight.Diffuse.x);
        config["Light Settings"]["Directional Light"]["Diffuse"].push_back(dirLight.Diffuse.y);
        config["Light Settings"]["Directional Light"]["Diffuse"].push_back(dirLight.Diffuse.z);

        config["Light Settings"]["Directional Light"]["Specular"].push_back(dirLight.Specular.x);
        config["Light Settings"]["Directional Light"]["Specular"].push_back(dirLight.Specular.y);
        config["Light Settings"]["Directional Light"]["Specular"].push_back(dirLight.Specular.z);


        config["Erosion Settings"]["Num Droplets"] = m_ErosionConfig.numDroplets;
        config["Erosion Settings"]["Max Steps"] = m_ErosionConfig.maxSteps;

        config["Erosion Settings"]["Deposition Rate"] = m_ErosionConfig.depositionRate;
        config["Erosion Settings"]["Erosion Rate"] = m_ErosionConfig.erosionRate;
        config["Erosion Settings"]["Evaporation Rate"] = m_ErosionConfig.evaporationRate;
        config["Erosion Settings"]["Min Slope"] = m_ErosionConfig.minSlope;
        config["Erosion Settings"]["Inertia"] = m_ErosionConfig.inertia;
        config["Erosion Settings"]["Particle Capacity"] = m_ErosionConfig.particleCapacity;
        config["Erosion Settings"]["Particle Radius"] = m_ErosionConfig.particleRadius;


        std::ofstream fout("config.yaml");
        fout << config;
    }
    void Application::loadConfig() {
        YAML::Node config = YAML::LoadFile("config.yaml");

        m_ViewDistance = config["Viewer Settings"]["View Distance"].as<int>();
        m_TestTerrainSize.x = config["Viewer Settings"]["Testing Size"][0].as<int>();
        m_TestTerrainSize.y = config["Viewer Settings"]["Testing Size"][1].as<int>();


        m_TerrainConfig.seed = config["Terrain Settings"]["Seed"].as<int>();
        m_TerrainConfig.size = config["Terrain Settings"]["Size"].as<int>();
        m_TerrainConfig.height = config["Terrain Settings"]["Height"].as<int>();
        m_TerrainConfig.resolution = config["Terrain Settings"]["Resolution"].as<int>();
        m_TerrainConfig.isoLevel = config["Terrain Settings"]["ISO Level"].as<float>();


        //Read in biomes
        int biomeCnt = config["Terrain Settings"]["Biomes"]["Count"].as<int>();
        m_TerrainConfig.biomes.resize(biomeCnt);
        for(int i = 0; i < biomeCnt; i++){
            m_TerrainConfig.biomes[i].name = config["Terrain Settings"]["Biomes"][i]["Name"].as<std::string>();
            m_TerrainConfig.biomes[i].height = config["Terrain Settings"]["Biomes"][i]["Height"].as<float>();

            m_TerrainConfig.biomes[i].color.x = config["Terrain Settings"]["Biomes"][i]["Color"][0].as<float>();
            m_TerrainConfig.biomes[i].color.y = config["Terrain Settings"]["Biomes"][i]["Color"][1].as<float>();
            m_TerrainConfig.biomes[i].color.z = config["Terrain Settings"]["Biomes"][i]["Color"][2].as<float>();
        }


        m_NoiseConfig.noiseOffset.x = config["Noise Settings"]["Offsets"][0].as<float>();
        m_NoiseConfig.noiseOffset.y = config["Noise Settings"]["Offsets"][1].as<float>();
        m_NoiseConfig.noiseOffset.z = config["Noise Settings"]["Offsets"][2].as<float>();

        m_NoiseConfig.noiseScale.x = config["Noise Settings"]["Noise Scale"][0].as<float>();
        m_NoiseConfig.noiseScale.y = config["Noise Settings"]["Noise Scale"][1].as<float>();

        m_NoiseConfig.exp = config["Noise Settings"]["EXP"].as<int>();
        m_NoiseConfig.heightMultiplier = config["Noise Settings"]["Height Multiplier"].as<float>();

        m_NoiseConfig.frequency = config["Noise Settings"]["Frequency"].as<float>();
        m_NoiseConfig.lacunarity = config["Noise Settings"]["Lacunarity"].as<float>();
        m_NoiseConfig.persistence = config["Noise Settings"]["Persistence"].as<float>();

        m_NoiseConfig.octaves = config["Noise Settings"]["Octaves"].as<int>();

        //LIGHTS
        dirLight.Direction.x = config["Light Settings"]["Directional Light"]["Direction"][0].as<float>();
        dirLight.Direction.y = config["Light Settings"]["Directional Light"]["Direction"][1].as<float>();
        dirLight.Direction.z = config["Light Settings"]["Directional Light"]["Direction"][2].as<float>();

        dirLight.Ambient.x = config["Light Settings"]["Directional Light"]["Ambient"][0].as<float>();
        dirLight.Ambient.y = config["Light Settings"]["Directional Light"]["Ambient"][1].as<float>();
        dirLight.Ambient.z = config["Light Settings"]["Directional Light"]["Ambient"][2].as<float>();

        dirLight.Diffuse.x = config["Light Settings"]["Directional Light"]["Diffuse"][0].as<float>();
        dirLight.Diffuse.y = config["Light Settings"]["Directional Light"]["Diffuse"][1].as<float>();
        dirLight.Diffuse.z = config["Light Settings"]["Directional Light"]["Diffuse"][2].as<float>();

        dirLight.Specular.x = config["Light Settings"]["Directional Light"]["Specular"][0].as<float>();
        dirLight.Specular.y = config["Light Settings"]["Directional Light"]["Specular"][1].as<float>();
        dirLight.Specular.z = config["Light Settings"]["Directional Light"]["Specular"][2].as<float>();


        m_ErosionConfig.numDroplets = config["Erosion Settings"]["Num Droplets"].as<int>();
        m_ErosionConfig.maxSteps = config["Erosion Settings"]["Max Steps"].as<int>();

        m_ErosionConfig.depositionRate = config["Erosion Settings"]["Deposition Rate"].as<float>();
        m_ErosionConfig.erosionRate = config["Erosion Settings"]["Erosion Rate"].as<float>();
        m_ErosionConfig.evaporationRate = config["Erosion Settings"]["Evaporation Rate"].as<float>();
        m_ErosionConfig.minSlope = config["Erosion Settings"]["Min Slope"].as<float>();
        m_ErosionConfig.inertia = config["Erosion Settings"]["Inertia"].as<float>();
        m_ErosionConfig.particleCapacity = config["Erosion Settings"]["Particle Capacity"].as<float>();
        m_ErosionConfig.particleRadius = config["Erosion Settings"]["Particle Radius"].as<int>();
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

    void Application::onWindowResize(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);

    }

}