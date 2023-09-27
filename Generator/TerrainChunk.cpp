//
// Created by apgra on 9/19/2023.
//

#include "TerrainChunk.h"
#include "../Engine/Helper/PerlinNoise.h"
#include "MarchingTables.h"


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
            height *= m_Config->heightMultiplier;



            vertices.emplace_back(xPos, height, zPos);

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

glm::vec3 TerrainChunk::interp(glm::vec3 edgeVertex1, float valueAtVertex1, glm::vec3 edgeVertex2, float valueAtVertex2)
{
    return (edgeVertex1 + (m_Config->isoLevel - valueAtVertex1) * (edgeVertex2 - edgeVertex1)  / (valueAtVertex2 - valueAtVertex1));
}

int TerrainChunk::indexFrom3D(int x, int y, int z){
    return x + m_Config->size * (y + m_Config->height * z);
}

int TerrainChunk::indexFrom3D(glm::ivec3 v) {
    return indexFrom3D(v.x, v.y, v.z);
}


void TerrainChunk::createMarchingCubesMesh() {
    std::vector<glm::vec3> vertices;

    int resolution = std::pow(2, m_Config->resolution);
    int size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));
    int height = m_Config->height + ((resolution - 1) * (m_Config->height - 1));

    //stored in XxZxY format
    std::vector<float> noiseValues(size * size * height);

    const siv::PerlinNoise::seed_type seed = m_Config->seed;
    const siv::PerlinNoise perlin{seed};


    for (int y = 0; y < height; y++) {
        for (int z = 0; z < size; z++) {
            for (int x = 0; x < size; x++) {
                float xPos = x / (float) (resolution);
                float zPos = z / (float) (resolution);
                float yPos = y / (float) (resolution);

                float noise = perlin.noise3D((xPos + m_Config->noiseOffset.x) / m_Config->noiseScale,
                                             (yPos + m_Config->noiseOffset.y) / m_Config->noiseScale,
                                             (zPos + m_Config->noiseOffset.z) / m_Config->noiseScale);
                noiseValues[indexFrom3D(x, y, z)] = (noise + 1) / 2.0f;
            }
        }
    }



    for (int y = 0; y < height - 1; y++) {
        for (int z = 0; z < size - 1; z++) {
            for (int x = 0; x < size - 1; x++) {
                float xPos = x / (float) (resolution);
                float zPos = z / (float) (resolution);
                float yPos = y / (float) (resolution);

                //Check if each vertex is within isoLevel to determine indices to use
                int cubeIndex = 0;
                if (noiseValues[indexFrom3D(x, y, z + 1)] < m_Config->isoLevel) cubeIndex |= 1;
                if (noiseValues[indexFrom3D(x + 1, y, z + 1)] < m_Config->isoLevel) cubeIndex |= 2;
                if (noiseValues[indexFrom3D(x + 1, y, z)] < m_Config->isoLevel) cubeIndex |= 4;
                if (noiseValues[indexFrom3D(x, y, z)] < m_Config->isoLevel) cubeIndex |= 8;

                if (noiseValues[indexFrom3D(x, y + 1, z + 1)] < m_Config->isoLevel) cubeIndex |= 16;
                if (noiseValues[indexFrom3D(x+1, y+1, z+1)] < m_Config->isoLevel) cubeIndex |= 32;
                if (noiseValues[indexFrom3D(x + 1, y + 1, z)] < m_Config->isoLevel) cubeIndex |= 64;
                if (noiseValues[indexFrom3D(x, y + 1, z)] < m_Config->isoLevel) cubeIndex |= 128;


                int *edges = triTable[cubeIndex];

                for (int i = 0; edges[i] != -1; i += 3) {
                    int e00 = edgeConnections[edges[i]][0];
                    int e01 = edgeConnections[edges[i]][1];

                    int e10 = edgeConnections[edges[i + 1]][0];
                    int e11 = edgeConnections[edges[i + 1]][1];

                    int e20 = edgeConnections[edges[i + 2]][0];
                    int e21 = edgeConnections[edges[i + 2]][1];


                    glm::ivec3 cubeCoords(x, y, z);
                    glm::vec3 cubePos(xPos, yPos, zPos);
                    vertices.push_back(
                            interp(cornerOffsets[e00],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e00])],
                                   cornerOffsets[e01],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e01])]) + cubePos);

                    vertices.push_back(
                            interp(cornerOffsets[e10],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e10])],
                                   cornerOffsets[e11],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e11])]) + cubePos);

                    vertices.push_back(
                            interp(cornerOffsets[e20],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e20])],
                                   cornerOffsets[e21],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e21])]) + cubePos);
                }
            }
        }


        if (!vertices.empty())
            m_Mesh->updateMeshData(vertices);
    }
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
