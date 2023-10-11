//
// Created by apgra on 10/11/2023.
//

#ifndef APPLICATION_TRANSFORM_H
#define APPLICATION_TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

class Transform{
public:
    Transform();
    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

    ~Transform();

    glm::mat4 getModelMatrix();

public:
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;
};

#endif //APPLICATION_TRANSFORM_H
