//
// Created by apgra on 9/11/2023.
//

#ifndef APPLICATION_RENDERER_H
#define APPLICATION_RENDERER_H


#include "Shader.h"

namespace Renderer {
    class Renderer {
    public:
        static void DrawSquare(const Shader& shader);
        static void DrawCube(const Shader& shader);
        static void Draw(unsigned int VAO, int count, const Shader& shader, bool drawIndexed = true);
    };
}


#endif //APPLICATION_RENDERER_H
