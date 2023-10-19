//
// Created by apgra on 9/19/2023.
//

#ifndef APPLICATION_TERRAINCHUNK_H
#define APPLICATION_TERRAINCHUNK_H


#include <memory>
#include "../Engine/Mesh.h"
#include "../Engine/MeshRenderer.h"
#include "../Engine/Transform.h"

enum GenerationType {
    HeightMap = 0,
    MarchingCube3D = 1
};
struct TerrainConfig {
    GenerationType genType = HeightMap;

    glm::vec3 color = {0.3294f, 0.7333f, 0.1921f};

    int seed = 32450;
    int size = 4;
    int resolution = 0;
    glm::vec3 noiseOffset = {0, 0, 0};
    glm::vec2 noiseScale = {1.4f, 1.4f};
    float heightMultiplier = 1;
    int octaves = 1;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float lacunarity = 1.0f;
    float persistence = 1.0f;

    //Marching Cubes
    int height = 2;
    float isoLevel = 0.5f;;
};


class TerrainChunk {
public:
    TerrainChunk(glm::vec3 pos, TerrainConfig* config);
    ~TerrainChunk();


private:
    glm::vec3 interp(glm::vec3 edgeVertex1, float valueAtVertex1, glm::vec3 edgeVertex2, float valueAtVertex2);

    void createHeightMapMesh();
    void createMarchingCubesMesh3D();

    int indexFrom3D(int x, int y, int z);
    int indexFrom3D(glm::ivec3 v);

    void createMesh();

public:
    void updateMesh();
    void draw();

private:
    TerrainConfig* m_Config;
    std::unique_ptr<Mesh> m_Mesh;
    std::unique_ptr<MeshRenderer> m_MeshRenderer;
    Transform m_Transform;

    int m_Size;
    int m_Height;
};


#endif //APPLICATION_TERRAINCHUNK_H
