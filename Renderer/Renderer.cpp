//
// Created by apgra on 9/11/2023.
//

#include "Renderer.h"
#include "VertexBuffer.h"

#include <glad/glad.h>

namespace Renderer {
    void Renderer::Draw(unsigned int VAO, int numIndices, const Shader &shader) {
        glBindVertexArray(VAO);
        shader.bind();
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
        shader.unbind();
        glBindVertexArray(0);
    }

    void Renderer::DrawSquare(const Shader& shader) {
        static const float vertices[] = {
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
        };
        static const unsigned int indices[6] = {
                0, 1, 2,
                2, 3, 0
        };

        static unsigned int VAO = -1, EBO = -1;
        static std::unique_ptr<VertexBuffer> VBO;

        if (VAO == -1) {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            VBO = std::make_unique<VertexBuffer>((void*)(&vertices));
            VBO->bind();

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);


            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void *) 0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, false, 5 * sizeof(float), (void *) (sizeof(float) * 3));
        }

        Draw(VAO, 6, shader);
    }

    void Renderer::DrawCube(const Shader &shader) {

    }
}
