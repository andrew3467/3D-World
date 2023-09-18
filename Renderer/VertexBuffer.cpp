//
// Created by apgra on 9/18/2023.
//

#include <glad/glad.h>
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(void *data) : m_ID(-1) {
    setData(data);
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::setData(void *data) {
    if(m_ID == -1){
        glGenBuffers(1, &m_ID);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_ID);
}
