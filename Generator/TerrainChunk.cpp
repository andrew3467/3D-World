//
// Created by apgra on 9/19/2023.
//

#include "TerrainChunk.h"
#include "MarchingTables.h"
#include <noise/noise.h>


TerrainChunk::TerrainChunk(TerrainConfig *config) : m_Config(config) {
    m_Mesh = std::make_unique<Mesh>();

    switch (m_Config->genType) {
        case HeightMap:
            createHeightMapMesh();
            break;
        case MarchingCube3D:
            createMarchingCubesMesh3D();
            break;
    }
}

TerrainChunk::~TerrainChunk() {

}

glm::vec3 TerrainChunk::interp(glm::vec3 edgeVertex1, float valueAtVertex1, glm::vec3 edgeVertex2, float valueAtVertex2)
{
    return (edgeVertex1 + (m_Config->isoLevel - valueAtVertex1) * (edgeVertex2 - edgeVertex1)  / (valueAtVertex2 - valueAtVertex1));
}

int TerrainChunk::indexFrom3D(int x, int y, int z){
    return x + m_Size * (y + m_Height * z);
}

int TerrainChunk::indexFrom3D(glm::ivec3 v) {
    return indexFrom3D(v.x, v.y, v.z);
}

int TerrainChunk::indexFrom2D(int x, int y) {
    return x * m_Size + y;
}


int TerrainChunk::indexFrom2D(glm::ivec3 v) {
    return indexFrom2D(v.x, v.z);
}

void TerrainChunk::createHeightMapMesh() {
    using namespace noise;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    int resolution = std::pow(2, m_Config->resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    m_Size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));

    module::Perlin noise;

    noise.SetSeed(m_Config->seed);
    noise.SetOctaveCount(m_Config->octaves);
    noise.SetFrequency(m_Config->frequency);
    noise.SetLacunarity(m_Config->lacunarity);

    int vertIndex = 0;
    for(int z = 0; z < m_Size; z++){
        for(int x = 0; x < m_Size; x++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            float height = noise.GetValue((xPos + m_Config->noiseOffset.x) / m_Config->noiseScale,
                                          0.0f,
                                           (zPos + m_Config->noiseOffset.y) / m_Config->noiseScale);
            //Normalize between 0 and 1
            height = (1 + height) / 2.0f;
            height *= m_Config->heightMultiplier;



            vertices.emplace_back(xPos, height, zPos);

            if (x < m_Size - 1 && z < m_Size - 1) {
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + m_Size);
                indices.push_back(vertIndex + 1);

                indices.push_back(vertIndex + m_Size);
                indices.push_back(vertIndex + m_Size + 1);
                indices.push_back(vertIndex + 1);
            }

            vertIndex++;
        }
    }

    m_Mesh->updateMeshData(vertices, indices);
}

void TerrainChunk::createMarchingCubesMesh3D() {
    using namespace noise;

    std::vector<glm::vec3> vertices;

    int resolution = std::pow(2, m_Config->resolution);
    m_Size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));
    m_Height = m_Config->height + ((resolution - 1) * (m_Config->height - 1));

    glm::vec3 scaledDim(1.0f / (float)resolution);

    //stored in XxZxY format
    std::vector<float> noiseValues(m_Size * m_Size * m_Height);

    module::Perlin noise;

    noise.SetSeed(m_Config->seed);
    noise.SetOctaveCount(m_Config->octaves);
    noise.SetFrequency(m_Config->frequency);
    noise.SetLacunarity(m_Config->lacunarity);


    for (int y = 0; y < m_Height; y++) {
        for (int z = 0; z < m_Size; z++) {
            for (int x = 0; x < m_Size; x++) {
                float xPos = x / (float) (resolution);
                float zPos = z / (float) (resolution);
                float yPos = y / (float) (resolution);


                float value = noise.GetValue((xPos + m_Config->noiseOffset.x) / m_Config->noiseScale,
                                             (yPos + m_Config->noiseOffset.y) / m_Config->noiseScale,
                                             (zPos + m_Config->noiseOffset.z) / m_Config->noiseScale);


                noiseValues[indexFrom3D(x, y, z)] = (value + 1) / 2.0f;
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


                    glm::vec3 cubeCoords(x, y, z);
                    glm::vec3 worldPos(xPos, yPos, zPos);


                    vertices.push_back(
                            interp(cornerOffsets[e00],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e00])],
                                   cornerOffsets[e01],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e01])]) * scaledDim + worldPos);

                    vertices.push_back(
                            interp(cornerOffsets[e10],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e10])],
                                   cornerOffsets[e11],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e11])]) * scaledDim + worldPos);

                    vertices.push_back(
                            interp(cornerOffsets[e20],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e20])],
                                   cornerOffsets[e21],
                                   noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e21])]) * scaledDim + worldPos);
                }
            }
        }
    }

    if (!vertices.empty())
        m_Mesh->updateMeshData(vertices);
}

void TerrainChunk::createMarchingCubesMesh2D() {
    using namespace noise;

    std::vector<glm::vec3> vertices;

    int resolution = std::pow(2, m_Config->resolution);
    m_Size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));

    glm::vec3 scaledDim(1.0f / (float) resolution);

    //stored in XxZxY format
    std::vector<float> noiseValues(m_Size * m_Size);

    module::Perlin noise;

    noise.SetSeed(m_Config->seed);
    noise.SetOctaveCount(m_Config->octaves);
    noise.SetFrequency(m_Config->frequency);
    noise.SetLacunarity(m_Config->lacunarity);

    for (int z = 0; z < m_Size; z++) {
        for (int x = 0; x < m_Size; x ++) {
            float xPos = z / (float) (resolution);
            float zPos = z / (float) (resolution);


            float value = noise.GetValue((xPos + m_Config->noiseOffset.x) / m_Config->noiseScale,
                                         0.0f,
                                         (zPos + m_Config->noiseOffset.z) / m_Config->noiseScale);


            noiseValues[indexFrom2D(z, z)] = (value + 1) / 2.0f;
        }
    }

    for (int z = 0; z < m_Size - 1; z++) {
        for (int x = 0; x < m_Size - 1; x++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            //Check if each vertex is within isoLevel to determine indices to use
            int cubeIndex = 0;
            if (noiseValues[indexFrom2D(x, z + 1)] < m_Config->isoLevel) cubeIndex |= 1;
            if (noiseValues[indexFrom2D(x + 1, z + 1)] < m_Config->isoLevel) cubeIndex |= 2;
            if (noiseValues[indexFrom2D(x + 1, z)] < m_Config->isoLevel) cubeIndex |= 4;
            if (noiseValues[indexFrom2D(x, z)] < m_Config->isoLevel) cubeIndex |= 8;

            if (noiseValues[indexFrom2D(x, z + 1)] < m_Config->isoLevel) cubeIndex |= 16;
            if (noiseValues[indexFrom2D(x + 1, z + 1)] < m_Config->isoLevel) cubeIndex |= 32;
            if (noiseValues[indexFrom2D(x + 1, z)] < m_Config->isoLevel) cubeIndex |= 64;
            if (noiseValues[indexFrom2D(x, z)] < m_Config->isoLevel) cubeIndex |= 128;

            int *edges = triTable[cubeIndex];

            for (int i = 0; edges[i] != -1; i += 3) {
                int e00 = edgeConnections[edges[i]][0];
                int e01 = edgeConnections[edges[i]][1];

                int e10 = edgeConnections[edges[i + 1]][0];
                int e11 = edgeConnections[edges[i + 1]][1];

                int e20 = edgeConnections[edges[i + 2]][0];
                int e21 = edgeConnections[edges[i + 2]][1];


                glm::vec3 cubeCoords(x, 0, z);
                glm::vec3 worldPos(xPos, 0, zPos);


                vertices.push_back(
                        interp(cornerOffsets[e00],
                               noiseValues[indexFrom2D(cubeCoords + cornerOffsets[e00])],
                               cornerOffsets[e01],
                               noiseValues[indexFrom2D(cubeCoords + cornerOffsets[e01])]) * scaledDim + worldPos);

                vertices.push_back(
                        interp(cornerOffsets[e10],
                               noiseValues[indexFrom2D(cubeCoords + cornerOffsets[e10])],
                               cornerOffsets[e11],
                               noiseValues[indexFrom2D(cubeCoords + cornerOffsets[e11])]) * scaledDim + worldPos);

                vertices.push_back(
                        interp(cornerOffsets[e20],
                               noiseValues[indexFrom2D(cubeCoords + cornerOffsets[e20])],
                               cornerOffsets[e21],
                               noiseValues[indexFrom2D(cubeCoords + cornerOffsets[e21])]) * scaledDim + worldPos);
            }
        }
    }

    if (!vertices.empty())
        m_Mesh->updateMeshData(vertices);

}

    void TerrainChunk::updateMesh() {
        switch (m_Config->genType) {
            case HeightMap:
                createHeightMapMesh();
                break;
            case MarchingCube3D:
                createMarchingCubesMesh3D();
                break;
            case MarchingCube2D:
                createMarchingCubesMesh2D();
                break;
        }
    }
