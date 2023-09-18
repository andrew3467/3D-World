//
// Created by apgra on 9/18/2023.
//

#ifndef APPLICATION_MESH_H
#define APPLICATION_MESH_H


#include <vector>
#include <glm/vec3.hpp>

#include "../Renderer/Shader.h"

struct MeshData{
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
};

class Mesh {
public:
    Mesh(const MeshData &data);
    Mesh();
    ~Mesh();

    Mesh(Mesh& other) = delete;

private:
    void setUpBuffers();

public:
    void draw(Renderer::Shader& shader);

private:
    std::vector<Mesh*> Meshes;

    std::vector<glm::vec3> m_Vertices;
    std::vector<unsigned int> m_Indices;

    unsigned int VAO, VBO, EBO;
};


#endif //APPLICATION_MESH_H
