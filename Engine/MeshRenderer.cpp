//
// Created by apgra on 10/11/2023.
//

#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(Mesh* mesh) : m_Mesh(mesh) {
    m_Shader = Shaders::solid_lit;
}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::draw(glm::mat4 modelMatrix) const {
    m_Shader->bind();
    m_Shader->setMat4("model", modelMatrix);
    m_Mesh->draw(*m_Shader);
}

void MeshRenderer::setShader(Shader *shader) {
    m_Shader = shader;
}
