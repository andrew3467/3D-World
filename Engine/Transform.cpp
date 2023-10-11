//
// Created by apgra on 10/11/2023.
//
#include <glm/ext/matrix_transform.hpp>
#include "Transform.h"

Transform::Transform()
    : Position(0.0f), Rotation(0.0f), Scale(1.0f)
{

}

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
    : Position(position), Rotation(rotation), Scale(scale)
{

}

Transform::~Transform() {

}

glm::mat4 Transform::getModelMatrix() {
    auto t = glm::translate(glm::mat4(1.0f), Position);
    auto s = glm::scale(t, Scale);
    auto r = glm::rotate(s, glm::radians(1.0f), Rotation);

    return s;
}

