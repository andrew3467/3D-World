//
// Created by apgra on 10/5/2023.
//

#ifndef APPLICATION_LIGHTS_H
#define APPLICATION_LIGHTS_H

#include <glm/vec3.hpp>

struct PointLight{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};

    glm::vec3 Ambient = {1.0f, 1.0f, 1.0f};
    glm::vec3 Diffuse;
    glm::vec3 Specular;

    float Constant = 0.5f;
    float Linear = 0.25f;
    float Exp = 0.125f;
};

struct DirectionalLight{
    glm::vec3 Direction = {-0.2f, -1.0f, -0.3f};

    glm::vec3 Ambient = {1.0f, 1.0f, 1.0f};
    glm::vec3 Diffuse = {0.5f, 0.5f, 0.5f};
    glm::vec3 Specular = {0.25f, 0.25f, 0.25f};
};

#endif //APPLICATION_LIGHTS_H
