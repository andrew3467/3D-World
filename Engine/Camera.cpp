//
// Created by apgra on 9/13/2023.
//

#include "Camera.h"

Camera::Camera(glm::vec3 initialPos, float moveSpeed)
    : camPos(initialPos), moveSpeed(moveSpeed) {
    //Assumes first m_Camera created is main m_Camera
    //if(MAIN_CAMERA == nullptr){
    //    MAIN_CAMERA = this;
    //}
}

Camera::~Camera() {

}

void Camera::move(Direction dir, float dt) {
    switch(dir){
        case Forward:
            camPos += camFront * moveSpeed * dt;
            break;
        case Backward:
            camPos -= camFront * moveSpeed * dt;
            break;
        case Right:
            camPos += glm::normalize(glm::cross(camFront, camUp)) * moveSpeed * dt;
            break;
        case Left:
            camPos -= glm::normalize(glm::cross(camFront, camUp)) * moveSpeed * dt;
            break;
    }
}
