//
// Created by apgra on 9/19/2023.
//

#ifndef APPLICATION_TERRAINCHUNK_H
#define APPLICATION_TERRAINCHUNK_H


#include <memory>
#include "../Engine/Mesh.h"

struct TerrainConfig{
    int size = 4;
    int resolution = 0;
    glm::vec2 noiseOffset = {0, 0};
    float noiseScale = 1.0f;
    float height = 1;
    int octaves = 1;
};

class TerrainChunk {
public:
    TerrainChunk(TerrainConfig* config);
    ~TerrainChunk();


private:
    void createMesh();

public:
    void updateMesh();

private:
    TerrainConfig* m_Config;
    std::unique_ptr<Mesh> m_Mesh;
};


#endif //APPLICATION_TERRAINCHUNK_H
