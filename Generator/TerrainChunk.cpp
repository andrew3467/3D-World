//
// Created by apgra on 9/19/2023.
//

#include "TerrainChunk.h"
#include "../Engine/Helper/PerlinNoise.h"


TerrainChunk::TerrainChunk(TerrainConfig *config) : m_Config(config) {
    m_Mesh = std::make_unique<Mesh>();

    switch (m_Config->genType) {
        case HeightMap:
            createHeightMapMesh();
            break;
        case MarchingCube:
            createMarchingCubesMesh();
            break;
    }
}

TerrainChunk::~TerrainChunk() {

}

void TerrainChunk::createHeightMapMesh() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    int resolution = std::pow(2, m_Config->resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    int size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));

    const siv::PerlinNoise::seed_type seed = m_Config->seed;
    const siv::PerlinNoise perlin{ seed };

    int vertIndex = 0;
    for(int z = 0; z < size; z++){
        for(int x = 0; x < size; x++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            float height = perlin.octave2D((xPos + m_Config->noiseOffset.x) / m_Config->noiseScale,
                                           (zPos + m_Config->noiseOffset.y) / m_Config->noiseScale,
                                           m_Config->octaves);
            //Normalize between 0 and 1
            height = (1 + height) / 2.0f;


            vertices.emplace_back(xPos, height * m_Config->heightMultiplier, zPos);

            if (x < size - 1 && z < size - 1) {
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + size);
                indices.push_back(vertIndex + 1);

                indices.push_back(vertIndex + size);
                indices.push_back(vertIndex + size + 1);
                indices.push_back(vertIndex + 1);
            }

            vertIndex++;
        }
    }

    m_Mesh->updateMeshData(vertices, indices);
}

void TerrainChunk::createMarchingCubesMesh() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    int resolution = std::pow(2, m_Config->resolution);
    int size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));
    int height = m_Config->height + ((resolution - 1) * (m_Config->height - 1));

    //stored in XxZxY format
    std::vector<std::vector<std::vector<float>>> noiseValues(size, std::vector<std::vector<float>>(size, std::vector<float>(height)));

    const siv::PerlinNoise::seed_type seed = m_Config->seed;
    const siv::PerlinNoise perlin{seed};


    for(int y = 0; y < height; y++) {
        for (int z = 0; z < size; z++) {
            for (int x = 0; x < size; x++) {
                float xPos = x / (float) (resolution);
                float zPos = z / (float) (resolution);
                float yPos = y / (float) (resolution);

                float noise = perlin.noise3D(xPos, zPos, yPos);
                noiseValues[x][z][y] = (noise + 1) / 2.0f;
            }
        }
    }


    //Determine isoLevel for each cube in mesh
    for(int y = 0; y < height - 1; y++) {
        for (int z = 0; z < size - 1; z++) {
            for (int x = 0; x < size - 1; x++) {
                //Check if each vertex is within isoLevel to determine indices to use
                int cubeIndex = 0;
                if(noiseValues[x][z][y] < m_Config->isoLevel) cubeIndex |= 1;
            }
        }
    }


    //m_Mesh->updateMeshData(vertices, indices);
}

void TerrainChunk::updateMesh() {
    switch (m_Config->genType) {
        case HeightMap:
            createHeightMapMesh();
            break;
        case MarchingCube:
            createMarchingCubesMesh();
            break;
    }
}
