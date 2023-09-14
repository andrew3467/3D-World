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

void Camera::rotate(float mouseX, float mouseY, bool cursorEnabled) {
    if (firstMouse)
    {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    float xoffset = mouseX - lastX;
    float yoffset = lastY - mouseY;

    lastX = mouseX;
    lastY = mouseY;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(direction);
}

void Camera::increaseMoveSpeed(float change) {
    moveSpeed += change;

    //Clamp movement to positive real numbers
    if(moveSpeed < 0){
        moveSpeed = 0;
    }
}
