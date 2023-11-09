//
// Created by apgra on 9/19/2023.
//

#include <thread>
#include "TerrainChunk.h"
#include "MarchingTables.h"
#include "../external/Simplex/SimplexNoise.h"
#include "HeightMapGenerator.h"
#include "ErosionSimulator.h"


TerrainChunk::TerrainChunk(glm::vec3 pos, TerrainConfig *config, NoiseConfig* noiseConfig, ErosionConfig* erosionConfig)
: m_TerrainConfig(config), m_ErosionConfig(erosionConfig), m_NoiseConfig(noiseConfig)
{
    m_Mesh = std::make_unique<Mesh>();
    m_MeshRenderer = std::make_unique<MeshRenderer>(m_Mesh.get());

    m_Transform.Position = pos * glm::vec3(m_TerrainConfig->size - 1, 0, m_TerrainConfig->size - 1);

    updateMesh();
}

TerrainChunk::~TerrainChunk() {

}

void TerrainChunk::draw(){
    m_MeshRenderer->draw(m_Transform.getModelMatrix());
}

glm::vec3 TerrainChunk::interp(glm::vec3 edgeVertex1, float valueAtVertex1, glm::vec3 edgeVertex2, float valueAtVertex2)
{
    return (edgeVertex1 + (m_TerrainConfig->isoLevel - valueAtVertex1) * (edgeVertex2 - edgeVertex1) / (valueAtVertex2 - valueAtVertex1));
}

int TerrainChunk::indexFrom3D(int x, int y, int z){
    return x + m_Size * (y + m_Height * z);
}
int TerrainChunk::indexFrom3D(glm::ivec3 v) {
    return indexFrom3D(v.x, v.y, v.z);
}


void TerrainChunk::createHeightMapMesh(bool erosionSim) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::vector<float>> map;

    if(m_TerrainConfig->genType == HeightMap){
        map = HeightMapGenerator::GenerateHeightMap(glm::vec2(m_Transform.Position.x, m_Transform.Position.z), *m_NoiseConfig, *m_TerrainConfig);
    }else{
        map = HeightMapGenerator::GenerateHeightMapfBm(glm::vec2(m_Transform.Position.x, m_Transform.Position.z), *m_NoiseConfig, *m_TerrainConfig);
    }

    if(erosionSim){
        ErosionSimulator::SimulateErosion2D(map, *m_ErosionConfig, m_TerrainConfig->seed);
    }


    int resolution = std::pow(2, m_TerrainConfig->resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    m_Size = m_TerrainConfig->size + ((resolution - 1) * (m_TerrainConfig->size - 1));

    //Create Vertices
    for(int z = 0, vertIndex = 0; z < m_Size; z++) {
        for (int x = 0; x < m_Size; x++, vertIndex++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            vertices.emplace_back(
                    glm::vec3(xPos, map[x][z], zPos),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec2((float)x / m_Size, (float)z / m_Size),
                    m_TerrainConfig->color
                    );

            if (x < m_Size - 1 && z < m_Size - 1) {
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + m_Size);
                indices.push_back(vertIndex + m_Size + 1);

                indices.push_back(vertIndex + m_Size + 1);
                indices.push_back(vertIndex + 1);
                indices.push_back(vertIndex);
            }
        }
    }

    m_Mesh->updateMeshData(vertices, indices);
}
void TerrainChunk::createMarchingCubesMesh3D() {
    std::vector<Vertex> vertices;

    int resolution = std::pow(2, m_TerrainConfig->resolution);
    m_Size = m_TerrainConfig->size + ((resolution - 1) * (m_TerrainConfig->size - 1));
    m_Height = m_TerrainConfig->height + ((resolution - 1) * (m_TerrainConfig->height - 1));

    glm::vec3 scaledDim(1.0f / (float) resolution);

    //stored in XxZxY format
    std::vector<float> noiseValues(m_Size * m_Size * m_Height);

    SimplexNoise simplexNoise;


    for (int y = 0; y < m_Height; y++) {
        for (int z = 0; z < m_Size; z++) {
            for (int x = 0; x < m_Size; x++) {
                float xPos = x / (float) (resolution);
                float zPos = z / (float) (resolution);
                float yPos = y / (float) (resolution);

                float noiseValue = 0.0f;
                float frequency = 1.01f;
                float amplitude = 1.0f;
                for(int i = 0; i < m_NoiseConfig->octaves; i++){
                    noiseValue += simplexNoise.noise((xPos + m_NoiseConfig->noiseOffset.x + m_Transform.Position.x) * m_NoiseConfig->noiseScale.x * frequency,
                                                     (yPos + m_NoiseConfig->noiseOffset.y + m_Transform.Position.y) * m_NoiseConfig->noiseScale.y * frequency,
                                                     (zPos + m_NoiseConfig->noiseOffset.z + m_Transform.Position.z) * m_NoiseConfig->noiseScale.x * frequency) * amplitude;

                    frequency *= 2.0f;
                    amplitude *= 0.5f;
                }


                noiseValues[indexFrom3D(x, y, z)] = noiseValue;
                //noiseValues[indexFrom3D(x, y, z)] = y - noiseValue;
            }
        }
    }


    for (int y = 0; y < m_Height - 1; y++) {
        for (int z = 0; z < m_Size - 1; z++) {
            for (int x = 0; x < m_Size - 1; x++) {
                float xPos = x / (float) (resolution);
                float zPos = z / (float) (resolution);
                float yPos = y / (float) (resolution);

                //Check if each vertex is within isoLevel to determine indices to use
                int cubeIndex = 0;
                if (noiseValues[indexFrom3D(x, y, z + 1)] < m_TerrainConfig->isoLevel) cubeIndex |= 1;
                if (noiseValues[indexFrom3D(x + 1, y, z + 1)] < m_TerrainConfig->isoLevel) cubeIndex |= 2;
                if (noiseValues[indexFrom3D(x + 1, y, z)] < m_TerrainConfig->isoLevel) cubeIndex |= 4;
                if (noiseValues[indexFrom3D(x, y, z)] < m_TerrainConfig->isoLevel) cubeIndex |= 8;

                if (noiseValues[indexFrom3D(x, y + 1, z + 1)] < m_TerrainConfig->isoLevel) cubeIndex |= 16;
                if (noiseValues[indexFrom3D(x + 1, y + 1, z + 1)] < m_TerrainConfig->isoLevel) cubeIndex |= 32;
                if (noiseValues[indexFrom3D(x + 1, y + 1, z)] < m_TerrainConfig->isoLevel) cubeIndex |= 64;
                if (noiseValues[indexFrom3D(x, y + 1, z)] < m_TerrainConfig->isoLevel) cubeIndex |= 128;


                int *edges = triTable[cubeIndex];

                for (int i = 0; edges[i] != -1; i += 3) {
                    int e00 = edgeConnections[edges[i]][0];
                    int e01 = edgeConnections[edges[i]][1];

                    int e10 = edgeConnections[edges[i + 1]][0];
                    int e11 = edgeConnections[edges[i + 1]][1];

                    int e20 = edgeConnections[edges[i + 2]][0];
                    int e21 = edgeConnections[edges[i + 2]][1];


                    glm::vec3 cubeCoords(x, y, z);
                    glm::vec3 worldPos(xPos, yPos, zPos);


                    glm::vec3 pos = interp(cornerOffsets[e00],
                                           noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e00])],
                                           cornerOffsets[e01],
                                           noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e01])]) * scaledDim +
                                    worldPos;

                    vertices.emplace_back(pos,
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec2(0.0f, 1.0f),
                                          m_TerrainConfig->color);


                    pos = interp(cornerOffsets[e10],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e10])],
                                 cornerOffsets[e11],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e11])]) * scaledDim + worldPos;

                    vertices.emplace_back(pos,
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec2(0.0f, 1.0f),
                                          m_TerrainConfig->color);

                    pos = interp(cornerOffsets[e20],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e20])],
                                 cornerOffsets[e21],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e21])]) * scaledDim + worldPos;

                    vertices.emplace_back(pos,
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec2(0.0f, 1.0f),
                                          m_TerrainConfig->color);
                }
            }
        }
    }

    if (!vertices.empty())
        m_Mesh->updateMeshData(vertices);
}

void TerrainChunk::createMesh(bool erosionSim){
    switch (m_TerrainConfig->genType) {
        case HeightMap:
            createHeightMapMesh(erosionSim);
            break;
        case HeightMapfBm:
            createHeightMapMesh(erosionSim);
            break;
        case MarchingCube3D:
            createMarchingCubesMesh3D();
            break;
    }
}

void TerrainChunk::updateMesh(bool erosionSim) {
    createMesh(erosionSim);
}