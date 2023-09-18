//
// Created by apgra on 9/18/2023.
//

#include "Mesh.h"
#include "../Renderer/Renderer.h"
#include <glad/glad.h>

Mesh::Mesh() {

}

Mesh::Mesh(const MeshData &data)
    : m_Vertices(data.vertices), m_Indices(data.indices), VAO(-1), VBO(-1), EBO(-1)
{
    setUpBuffers();
    Meshes.push_back(this);
}

void Mesh::setUpBuffers() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(glm::vec3), &m_Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
}

void Mesh::draw(Renderer::Shader &shader) {
    Renderer::Renderer::Draw(VAO, m_Indices.size(), shader);
}

Mesh::~Mesh() {

}


