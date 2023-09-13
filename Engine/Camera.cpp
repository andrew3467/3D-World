//
// Created by apgra on 9/13/2023.
//

#include "Camera.h"

Camera::Camera(glm::vec3 initialPos, float moveSpeed)
    : pos(initialPos), moveSpeed(moveSpeed) {
    //Assumes first camera created is main camera
    if(MAIN_CAMERA == nullptr){
        MAIN_CAMERA = this;
    }
}

Camera::~Camera() {

}

glm::mat4 Camera::projection() {
    return glm::mat4();
}

glm::mat4 Camera::view() {
    return glm::mat4();
}

glm::mat4 Camera::viewProjection() {
    return view() * projection();
}
