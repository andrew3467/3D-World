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
    glm::ivec2 size = m_Config->size + (glm::ivec2(resolution - 1) * (m_Config->size - glm::ivec2(1)));

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{ seed };

    int vertIndex = 0;
    for(int z = 0; z < size.y; z++){
        for(int x = 0; x < size.x; x++){
            float xPos = x / (float)(resolution);
            float zPos = z / (float)(resolution);


            const float height = perlin.octave2D(xPos + m_Config->noiseOffset.x,
                                                 zPos + m_Config->noiseOffset.y,
                                                 m_Config->octaves);
            vertices.emplace_back(xPos, height, zPos);

            if(x < size.x - 1 && z < size.y - 1){
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + size.x);
                indices.push_back(vertIndex + 1);

                indices.push_back(vertIndex + size.x);
                indices.push_back(vertIndex + size.x + 1);
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
