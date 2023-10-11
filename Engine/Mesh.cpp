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

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), VAO(0), VBO(0), EBO(0)
{
    setUpBuffers();
    Meshes.push_back(this);
}

Mesh::Mesh(std::vector<Vertex> vertices) : m_Vertices(std::move(vertices)), VAO(0), VBO(0), EBO(0){
    setUpBuffers();
    Meshes.push_back(this);
}

void Mesh::setUpBuffers() {
    if(VAO == 0) glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    if(VBO == 0) glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

    if(indexedBuffer){
        if (EBO == 0) glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, UV));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, Color));
}

Mesh::~Mesh() {

}

void Mesh::updateMeshData(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
    m_Vertices.clear();
    m_Indices.clear();

    m_Vertices = vertices;
    m_Indices =  indices;

    indexedBuffer = true;
    setUpBuffers();
}

void Mesh::updateMeshData(std::vector<Vertex> &vertices) {
    m_Vertices.clear();
    m_Vertices = vertices;

    indexedBuffer = false;
    setUpBuffers();
}

void Mesh::DrawMeshes(Renderer::Shader &shader) {
    for(auto& mesh : Meshes){
        mesh->draw(shader);
    }
}

void Mesh::draw(Renderer::Shader &shader) {
    if(indexedBuffer){
        Renderer::Renderer::Draw(VAO, m_Indices.size(), shader, true);
    }
    else{
        Renderer::Renderer::Draw(VAO, m_Vertices.size(), shader, false);
    }
}
