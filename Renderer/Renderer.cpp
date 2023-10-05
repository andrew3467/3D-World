//
// Created by apgra on 9/11/2023.
//

#include "Renderer.h"

#include <glad/glad.h>

namespace Renderer {
    void Renderer::Draw(unsigned int VAO, int count, const Shader& shader, bool drawIndexed) {
        glBindVertexArray(VAO);
        shader.bind();

        if(drawIndexed){
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        }else{
            glDrawArrays(GL_TRIANGLES, 0, count);
        }

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

        static unsigned int VAO = -1, VBO = -1, EBO = -1;

        if (VAO == -1) {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

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
        static unsigned int VAO = -1, VBO = -1, EBO = -1;

        if (VAO == -1) {
            static float vertices[] = {
                    -0.5, -0.5,  0.5, //0
                    0.5, -0.5,  0.5, //1
                    -0.5,  0.5,  0.5, //2
                    0.5,  0.5,  0.5, //3
                    -0.5, -0.5, -0.5, //4
                    0.5, -0.5, -0.5, //5
                    -0.5,  0.5, -0.5, //6
                    0.5,  0.5, -0.5  //7
            };

            static unsigned int indices[36] = {
                    //Top
                    2, 6, 7,
                    2, 3, 7,

                    //Bottom
                    0, 4, 5,
                    0, 1, 5,

                    //Left
                    0, 2, 6,
                    0, 4, 6,

                    //Right
                    1, 3, 7,
                    1, 5, 7,

                    //Front
                    0, 2, 3,
                    0, 1, 3,

                    //Back
                    4, 6, 7,
                    4, 5, 7
            };


            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);


            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void *) 0);
        }

        Draw(VAO, 36, shader);
    }
}
