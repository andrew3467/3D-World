//
// Created by apgra on 9/13/2023.
//

#ifndef APPLICATION_CAMERA_H
#define APPLICATION_CAMERA_H


#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

enum Direction{
    Forward = 0,
    Backward = 1,
    Left = 2,
    Right = 3
};

class Camera {
public:
    //static Camera *MAIN_CAMERA;

    Camera(glm::vec3 initialPos, float moveSpeed);
    ~Camera();

    Camera(Camera& other) = delete;

public:
    void move(Direction dir, float dt);
    void rotate(float mouseX, float mouseY, bool cursorEnabled = true);

    void increaseMoveSpeed(float change);


    inline float GetYaw() const { return yaw;}
    inline float GetPitch() const { return pitch;}

    inline glm::mat4 viewProjection(glm::vec2 windowSize) const {return projection(windowSize) * view();}
    inline glm::mat4 projection(glm::vec2 windowSize) const {return glm::perspective(glm::radians(60.0f),
                                                                 windowSize.x / windowSize.y,0.1f, 100.0f);}
    inline glm::mat4 view() const {return glm::lookAt(camPos, center(), camUp);}
    inline glm::vec3 position() const {return camPos;}
    inline glm::vec3 front() const {return camFront;}
    inline glm::vec3 center() const {return camPos + camFront;}
    inline glm::vec3 cameraUp() const {return camUp;}


private:
    glm::vec3 camPos;
    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camDir = glm::normalize(camPos - camTarget);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camRight = glm::normalize(glm::cross(up, camDir));
    glm::vec3 camUp = glm::cross(camDir, camRight);

    float moveSpeed;

    const float sensitivity = 0.1f;
    bool firstMouse = true;
    float lastX, lastY;
    float yaw = -90.0f;
    float pitch = 0.0f;
};


#endif //APPLICATION_CAMERA_H
