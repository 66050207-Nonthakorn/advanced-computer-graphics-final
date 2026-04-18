#include "Engine/Camera.hpp"

Camera::Camera(float fovDegrees, float aspect, float nearPlane, float farPlane)
    : position(0, 0, 3), target(0, 0, 0), up(0, 1, 0),
      fov(fovDegrees), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane) {}

glm::mat4 Camera::getView() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjection() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}
