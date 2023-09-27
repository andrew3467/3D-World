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
    glm::vec3 noiseOffset = {0, 0, 0};
    float noiseScale = 1.4f;
    float heightMultiplier = 1;
    int octaves = 1;

    //Marching Cubes
    int height = 2;
    float isoLevel = 0.5f;
};


class TerrainChunk {
public:
    TerrainChunk(TerrainConfig* config);
    ~TerrainChunk();


private:
    glm::vec3 interp(glm::vec3 edgeVertex1, float valueAtVertex1, glm::vec3 edgeVertex2, float valueAtVertex2);

    void createHeightMapMesh();
    void createMarchingCubesMesh();

    int indexFrom3D(int x, int y, int z);
    int indexFrom3D(glm::ivec3 v);

public:
    void updateMesh();

private:
    TerrainConfig* m_Config;
    std::unique_ptr<Mesh> m_Mesh;
};


#endif //APPLICATION_TERRAINCHUNK_H
