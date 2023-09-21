//
// Created by apgra on 9/18/2023.
//

#include "Mesh.h"
#include "../Renderer/Renderer.h"
#include <glad/glad.h>

#include <utility>


std::vector<Mesh*> Mesh::Meshes = std::vector<Mesh*>();

Mesh::Mesh() : VAO(0), VBO(0), EBO(0) {
    Meshes.push_back(this);
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices)
        : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), VAO(0), VBO(0), EBO(0)
{
    setUpBuffers();
    Meshes.push_back(this);
}

void Mesh::setUpBuffers() {
    if(VAO == 0) glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    if(VBO == 0) glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(glm::vec3), &m_Vertices[0], GL_STATIC_DRAW);

    if(EBO == 0) glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
}

Mesh::~Mesh() {

}

void Mesh::updateMeshData(std::vector<glm::vec3> &vertices, std::vector<unsigned int> &indices) {
    m_Vertices = vertices;
    m_Indices =  indices;

    setUpBuffers();
}

void Mesh::DrawMeshes(Renderer::Shader &shader) {
    for(auto& mesh : Meshes){
        mesh->draw(shader);
    }
}

void Mesh::draw(Renderer::Shader &shader) {
    Renderer::Renderer::Draw(VAO, m_Indices.size(), shader);
}