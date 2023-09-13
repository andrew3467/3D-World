//
// Created by apgra on 9/13/2023.
//

#ifndef APPLICATION_CAMERA_H
#define APPLICATION_CAMERA_H


#include <glm/glm.hpp>

class Camera {
public:
    static Camera *MAIN_CAMERA;

    Camera(glm::vec3 initialPos, float moveSpeed);
    ~Camera();

    Camera(Camera& other) = delete;

public:
    glm::mat4 projection();
    glm::mat4 view();
    glm::mat4 viewProjection();


private:
    glm::vec3 pos;
    glm::vec3 front;
    float moveSpeed;
};


#endif //APPLICATION_CAMERA_H
