//
// Created by apgra on 9/19/2023.
//

#include <thread>
#include "TerrainChunk.h"
#include "MarchingTables.h"
#include "../external/Simplex/SimplexNoise.h"


TerrainChunk::TerrainChunk(glm::vec3 pos, TerrainConfig *config) : m_Config(config) {
    m_Mesh = std::make_unique<Mesh>();
    m_MeshRenderer = std::make_unique<MeshRenderer>(m_Mesh.get());

    m_Transform.Position = pos * glm::vec3(m_Config->size - 1, 0, m_Config->size - 1);

    updateMesh();
}

TerrainChunk::~TerrainChunk() {

}

void TerrainChunk::draw(){
    m_MeshRenderer->draw(m_Transform.getModelMatrix());
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


void TerrainChunk::createHeightMapMesh() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int resolution = std::pow(2, m_Config->resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    m_Size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));

    SimplexNoise simplexNoise(
            m_Config->frequency,
            m_Config->amplitude,
            m_Config->lacunarity,
            m_Config->persistence
    );

    int vertIndex = 0;
    for(int z = 0; z < m_Size; z++){
        for(int x = 0; x < m_Size; x++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            float height = simplexNoise.fractal(m_Config->octaves,
                                        (xPos + m_Config->noiseOffset.x + m_Transform.Position.x) * m_Config->noiseScale.x,
                                           (zPos + m_Config->noiseOffset.y + m_Transform.Position.z) * m_Config->noiseScale.x);

            //Normalize between 0 and 1
            height = (1 + height) / 2.0f;
            height *= m_Config->heightMultiplier;


            vertices.emplace_back(glm::vec3(xPos, height, zPos),
                                  glm::vec3(0.0f, 0.0f, 0.0f),
                                  glm::vec2((float)x / m_Size, (float)z / m_Size),
                                  m_Config->color);

            if (x < m_Size - 1 && z < m_Size - 1) {
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + m_Size);
                indices.push_back(vertIndex + m_Size + 1);

                indices.push_back(vertIndex + m_Size + 1);
                indices.push_back(vertIndex + 1);
                indices.push_back(vertIndex);
            }

            vertIndex++;
        }
    }

    //Loop over each triangle, calculating normals
    //For each face calculate cross product
    //Add each result to normal of each vertex in face
    //Finalize by normalizing normals
    for(int i = 0; i < indices.size(); i+=3){
        glm::vec3 v0 = vertices[indices[i]].Position;
        glm::vec3 v1 = vertices[indices[i+1]].Position;
        glm::vec3 v2 = vertices[indices[i+2]].Position;

        glm::vec3 e0 = v0 - v1;
        glm::vec3 e1 = v0 - v2;

        glm::vec3 e0crosse1 = glm::cross(e0, e1);

        vertices[indices[i]].Normal += e0crosse1;
        vertices[indices[i+1]].Normal += e0crosse1;
        vertices[indices[i+2]].Normal += e0crosse1;
    }

    //Normalize Normals
    for(auto& v : vertices){
        v.Normal = glm::normalize(v.Normal);
    }


    m_Mesh->updateMeshData(vertices, indices);
}
void TerrainChunk::createMarchingCubesMesh3D() {
    std::vector<Vertex> vertices;

    int resolution = std::pow(2, m_Config->resolution);
    m_Size = m_Config->size + ((resolution - 1) * (m_Config->size - 1));
    m_Height = m_Config->height + ((resolution - 1) * (m_Config->height - 1));

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
                for(int i = 0; i < m_Config->octaves; i++){
                    noiseValue += simplexNoise.noise((xPos + m_Config->noiseOffset.x + m_Transform.Position.x) * m_Config->noiseScale.x * frequency,
                                                            (yPos + m_Config->noiseOffset.y + m_Transform.Position.y) * m_Config->noiseScale.y * frequency,
                                                            (zPos + m_Config->noiseOffset.z + m_Transform.Position.z) * m_Config->noiseScale.x * frequency) * amplitude;

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
                if (noiseValues[indexFrom3D(x, y, z + 1)] < m_Config->isoLevel) cubeIndex |= 1;
                if (noiseValues[indexFrom3D(x + 1, y, z + 1)] < m_Config->isoLevel) cubeIndex |= 2;
                if (noiseValues[indexFrom3D(x + 1, y, z)] < m_Config->isoLevel) cubeIndex |= 4;
                if (noiseValues[indexFrom3D(x, y, z)] < m_Config->isoLevel) cubeIndex |= 8;

                if (noiseValues[indexFrom3D(x, y + 1, z + 1)] < m_Config->isoLevel) cubeIndex |= 16;
                if (noiseValues[indexFrom3D(x + 1, y + 1, z + 1)] < m_Config->isoLevel) cubeIndex |= 32;
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


                    glm::vec3 pos = interp(cornerOffsets[e00],
                                           noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e00])],
                                           cornerOffsets[e01],
                                           noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e01])]) * scaledDim +
                                    worldPos;

                    vertices.emplace_back(pos,
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec2(0.0f, 1.0f),
                                          m_Config->color);


                    pos = interp(cornerOffsets[e10],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e10])],
                                 cornerOffsets[e11],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e11])]) * scaledDim + worldPos;

                    vertices.emplace_back(pos,
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec2(0.0f, 1.0f),
                                          m_Config->color);

                    pos = interp(cornerOffsets[e20],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e20])],
                                 cornerOffsets[e21],
                                 noiseValues[indexFrom3D(cubeCoords + cornerOffsets[e21])]) * scaledDim + worldPos;

                    vertices.emplace_back(pos,
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec2(0.0f, 1.0f),
                                          m_Config->color);
                }
            }
        }
    }

    //Update Normals
    for (int i = 0; i < vertices.size(); i += 3) {
        glm::vec3 v0 = vertices[i].Position;
        glm::vec3 v1 = vertices[i + 1].Position;
        glm::vec3 v2 = vertices[i + 2].Position;

        glm::vec3 e0 = v0 - v1;
        glm::vec3 e1 = v0 - v2;

        glm::vec3 e0crosse1 = glm::cross(e0, e1);

        vertices[i].Normal += e0crosse1;
        vertices[i + 1].Normal += e0crosse1;
        vertices[i + 2].Normal += e0crosse1;
    }

    //Normalize Normals
    for (auto &v: vertices) {
        v.Normal = glm::normalize(v.Normal);
    }

    if (!vertices.empty())
        m_Mesh->updateMeshData(vertices);
}

void TerrainChunk::updateMesh() {
    std::thread thread(&TerrainChunk::createHeightMapMesh, this);
    thread.join();
    return;

    switch (m_Config->genType) {
        case HeightMap:
            createHeightMapMesh();
            break;
        case MarchingCube3D:
            createMarchingCubesMesh3D();
            break;
    }
}