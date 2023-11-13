//
// Created by apgra on 9/19/2023.
//

#ifndef APPLICATION_TERRAINCHUNK_H
#define APPLICATION_TERRAINCHUNK_H



#include <memory>
#include <utility>
#include "../Engine/Mesh.h"
#include "../Engine/MeshRenderer.h"

#include "../Engine/Transform.h"

enum GenerationType {
    HeightMap = 0,
    HeightMapfBm = 1,
    MarchingCube3D = 2
};

struct Biome{
    Biome() : name("New Biome"), height(0.5f) {

    }

    std::string name;
    float height;
    glm::vec3 color{0.0f};
};

struct TerrainConfig {
    GenerationType genType = HeightMapfBm;

    int seed = 32450;
    int size = 4;
    int resolution = 0;

    std::vector<Biome> biomes;

    //Marching Cubes
    int height = 2;
    float isoLevel = 0.5f;;
};

struct NoiseConfig{
    glm::vec3 noiseOffset = {0, 0, 0};
    glm::vec2 noiseScale = {1.4f, 1.4f};
    int octaves = 1;

    float heightMultiplier = 2.0f;
    int exp = 1;

    float frequency = 1.0f;
    float lacunarity = 1.0f;
    float persistence = 1.0f;
};

struct ErosionConfig {
    int numDroplets = 50;
    int maxSteps = 100;

    float depositionRate = 0.25f;
    float erosionRate = 0.25f;
    float evaporationRate = 0.25f;
    float minSlope = 0.05f;

    float inertia = 0.5f;

    float particleCapacity = 1.0f;
    int particleRadius = 1;
};


class TerrainChunk {
public:
    TerrainChunk(glm::vec3 pos, TerrainConfig* terrainConfig, NoiseConfig* noiseConfig, ErosionConfig* erosionConfig);
    ~TerrainChunk();


private:
    glm::vec3 interp(glm::vec3 edgeVertex1, float valueAtVertex1, glm::vec3 edgeVertex2, float valueAtVertex2);

    void createHeightMapMesh(bool erosionSim = false);
    void createMarchingCubesMesh3D();

    int indexFrom3D(int x, int y, int z);
    int indexFrom3D(glm::ivec3 v);

    void createMesh(bool erosionSim = false);

public:
    void updateMesh(bool erosionSim = false);
    void draw();

private:
    TerrainConfig* m_TerrainConfig;
    NoiseConfig* m_NoiseConfig;
    ErosionConfig* m_ErosionConfig;


    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<MeshRenderer> m_MeshRenderer;
    Transform m_Transform;

    int m_Size;
    int m_Height;
};


#endif //APPLICATION_TERRAINCHUNK_H
