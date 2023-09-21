//
// Created by apgra on 9/19/2023.
//

#include "TerrainChunk.h"
#include "../Engine/Helper/PerlinNoise.h"


TerrainChunk::TerrainChunk(TerrainConfig *config) : m_Config(config) {
    m_Mesh = std::make_unique<Mesh>();
    createMesh();
}

TerrainChunk::~TerrainChunk() {

}

void TerrainChunk::createMesh() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    int resolution = std::pow(2, m_Config->resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    int size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));
    int halfSize = size / 2;

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{ seed };

    int vertIndex = 0;
    for(int z = -halfSize; z < halfSize; z++){
        for(int x = -halfSize; x < halfSize; x++){
            float xPos = x / (float)(resolution);
            float zPos = z / (float)(resolution);


            float height = perlin.octave2D((xPos + m_Config->noiseOffset.x) / m_Config->noiseScale,
                                                 (zPos + m_Config->noiseOffset.y) / m_Config->noiseScale,
                                                 m_Config->octaves);
            //Normalize height between 0 and 1
            height = (1 + height) / 2.0f;


            vertices.emplace_back(xPos, height * m_Config->height, zPos);

            if(x < size - 1 && z < size - 1) {
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

void TerrainChunk::updateMesh() {
    createMesh();
}
