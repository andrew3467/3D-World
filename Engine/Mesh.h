//
// Created by apgra on 9/18/2023.
//

#ifndef APPLICATION_MESH_H
#define APPLICATION_MESH_H


#include <vector>
#include <glm/vec3.hpp>

#include "../Renderer/Shader.h"


class Mesh {
public:
    Mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices);
    Mesh();
    ~Mesh();

    Mesh(Mesh& other) = delete;

private:
    void setUpBuffers();

public:
    static void DrawMeshes(Renderer::Shader& shader);

    void draw(Renderer::Shader& shader);
    void updateMeshData(std::vector<glm::vec3> &vertices, std::vector<unsigned int> &indices);

private:
    std::vector<glm::vec3> m_Vertices;
    std::vector<unsigned int> m_Indices;

    unsigned int VAO, VBO, EBO;

public:
    static std::vector<Mesh*> Meshes;
};


#endif //APPLICATION_MESH_H
