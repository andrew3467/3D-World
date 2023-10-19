//
// Created by apgra on 9/18/2023.
//

#include "Mesh.h"
#include "../Renderer/Renderer.h"
#include <glad/glad.h>

#include <utility>


Mesh::Mesh() : VAO(0), VBO(0), EBO(0) {
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), VAO(0), VBO(0), EBO(0)
{
    setUpBuffers();
}

Mesh::Mesh(std::vector<Vertex> vertices) : m_Vertices(std::move(vertices)), VAO(0), VBO(0), EBO(0){
    setUpBuffers();
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

    recalculateNormals();

    indexedBuffer = true;
    setUpBuffers();
}

void Mesh::updateMeshData(std::vector<Vertex> &vertices) {
    m_Vertices.clear();
    m_Vertices = vertices;

    recalculateNormals();

    indexedBuffer = false;
    setUpBuffers();
}

void Mesh::draw(Shader &shader) {
    if(indexedBuffer){
        Renderer::Renderer::Draw(VAO, m_Indices.size(), shader, true);
    }
    else{
        Renderer::Renderer::Draw(VAO, m_Vertices.size(), shader, false);
    }
}

void Mesh::recalculateNormals() {
    //Loop over each triangle, calculating normals
    //For each face calculate cross product
    //Add each result to normal of each vertex in face
    //Finalize by normalizing normals
    if(indexedBuffer){
        for (int i = 0; i < m_Indices.size(); i += 3) {
            glm::vec3 v0 = m_Vertices[m_Indices[i]].Position;
            glm::vec3 v1 = m_Vertices[m_Indices[i + 1]].Position;
            glm::vec3 v2 = m_Vertices[m_Indices[i + 2]].Position;

            glm::vec3 e0 = v0 - v1;
            glm::vec3 e1 = v0 - v2;

            glm::vec3 e0crosse1 = glm::cross(e0, e1);

            m_Vertices[m_Indices[i]].Normal += e0crosse1;
            m_Vertices[m_Indices[i + 1]].Normal += e0crosse1;
            m_Vertices[m_Indices[i + 2]].Normal += e0crosse1;
        }
    }else{
        for(int i = 0; i < m_Vertices.size(); i+=3){
            glm::vec3 v0 = m_Vertices[i].Position;
            glm::vec3 v1 = m_Vertices[i + 1].Position;
            glm::vec3 v2 = m_Vertices[i + 2].Position;

            glm::vec3 e0 = v0 - v1;
            glm::vec3 e1 = v0 - v2;

            glm::vec3 e0crosse1 = glm::cross(e0, e1);

            m_Vertices[i].Normal += e0crosse1;
            m_Vertices[i + 1].Normal += e0crosse1;
            m_Vertices[i + 2].Normal += e0crosse1;
        }
    }

    //Normalize Normals
    for(auto& v : m_Vertices){
        v.Normal = glm::normalize(v.Normal);
    }
}
