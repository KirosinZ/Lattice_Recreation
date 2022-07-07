#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

FirstPersonFlatCamera::Camera::Camera()
:   position(glm::vec3(0.0f)),
    globalUp(glm::vec3(0.0f, 1.0f, 0.0f))
{
    updateVectors();
}

[[maybe_unused]] FirstPersonFlatCamera::Camera::Camera(const glm::vec3 pos, const glm::vec3 up, const float yaw, const float pitch)
:   position(pos),
    globalUp(up),
    yaw(yaw),
    pitch(pitch)
{
    updateVectors();
}

void FirstPersonFlatCamera::Camera::updateVectors()
{
    localFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    localFront.y = sin(glm::radians(pitch));
    localFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    localFront = glm::normalize(localFront);

    localRight = glm::normalize(glm::cross(localFront, globalUp));
    localUp  = glm::normalize(glm::cross(localRight, localFront));
}

glm::mat4 FirstPersonFlatCamera::Camera::view() const
{
    return glm::lookAt(position, position + localFront, localUp);
}

void FirstPersonFlatCamera::Camera::processKeyboard(const CameraMovement direction, const float deltaTime)
{
    if (direction == CameraMovement::cNone)
        return;

    const float velocity = moveSpeed * deltaTime;
    glm::vec3 dir = glm::vec3(0.0f);

    if ((direction & CameraMovement::cForward) == CameraMovement::cForward) {
        glm::vec3 tmp(localFront.x, 0.0f, localFront.z);
        tmp = glm::normalize(tmp);
        dir += tmp;
        dir.y = 0.0f;
    }
    if ((direction & CameraMovement::cBackward) == CameraMovement::cBackward) {
        glm::vec3 tmp(localFront.x, 0.0f, localFront.z);
        tmp = glm::normalize(tmp);
        dir -= tmp;
        dir.y = 0.0f;
    }
    if ((direction & CameraMovement::cLeft) == CameraMovement::cLeft)
        dir -= localRight;
    if ((direction & CameraMovement::cRight) == CameraMovement::cRight)
        dir += localRight;
    if ((direction & CameraMovement::cUp) == CameraMovement::cUp)
        dir += globalUp;
    if ((direction & CameraMovement::cDown) == CameraMovement::cDown)
        dir -= globalUp;

    position += dir * velocity;
}

void FirstPersonFlatCamera::Camera::processMouse(float xOffset, float yOffset, const GLboolean constrainPitch)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw   += xOffset;
    pitch += yOffset;

    if (constrainPitch)
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}