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

    const siv::PerlinNoise::seed_type seed = 123456u;

    const siv::PerlinNoise perlin{ seed };

    int vertIndex = 0;
    for(int z = 0; z < m_Config->size.y; z++){
        for(int x = 0; x < m_Config->size.x; x++){
            const float height = perlin.octave2D(x + m_Config->noiseOffset.x, z + m_Config->noiseOffset.y, m_Config->octaves);
            vertices.emplace_back(x, height, z);

            if(x < m_Config->size.x - 1 && z < m_Config->size.y - 1){
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + m_Config->size.x);
                indices.push_back(vertIndex + 1);

                indices.push_back(vertIndex + m_Config->size.x);
                indices.push_back(vertIndex + m_Config->size.x + 1);
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
