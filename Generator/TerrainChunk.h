//
// Created by apgra on 9/19/2023.
//

#ifndef APPLICATION_TERRAINCHUNK_H
#define APPLICATION_TERRAINCHUNK_H


#include <memory>
#include "../Engine/Mesh.h"

enum GenerationType {
    HeightMap = 0,
    MarchingCube = 1
};

struct TerrainConfig {
    GenerationType genType = HeightMap;

    int seed = 32450;
    int size = 4;
    int resolution = 0;
    glm::vec2 noiseOffset = {0, 0};
    float noiseScale = 1.0f;
    float heightMultiplier = 1;
    int octaves = 1;

    //Marching Cubes
    int height = 1;
    float isoLevel = 0.5f;
};


class TerrainChunk {
public:
    TerrainChunk(TerrainConfig* config);
    ~TerrainChunk();


private:
    void createHeightMapMesh();
    void createMarchingCubesMesh();

public:
    void updateMesh();

private:
    TerrainConfig* m_Config;
    std::unique_ptr<Mesh> m_Mesh;
};


#endif //APPLICATION_TERRAINCHUNK_H
