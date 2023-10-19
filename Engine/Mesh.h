//
// Created by apgra on 9/18/2023.
//

#ifndef APPLICATION_MESH_H
#define APPLICATION_MESH_H


#include <vector>
#include <glm/vec3.hpp>

#include "../Renderer/Shader.h"


struct Vertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UV;
    glm::vec3 Color;

    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec3 c)
        : Position(p), Normal(n), UV(u), Color(c)
    {

    }
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh(std::vector<Vertex> vertices);
    Mesh();
    ~Mesh();

    Mesh(Mesh& other) = delete;

private:
    void setUpBuffers();

public:
    void draw(Shader& shader);
    void updateMeshData(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);
    void updateMeshData(std::vector<Vertex> &vertices);

    void recalculateNormals();

private:
    bool indexedBuffer = true;

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    unsigned int VAO, VBO, EBO;
};


#endif //APPLICATION_MESH_H
