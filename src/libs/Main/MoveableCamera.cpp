#include "MoveableCamera.hpp"
#include "Engine/Manager/InputManager.hpp"

MovableCamera::MovableCamera(float fovDegrees, float aspect, float nearPlane, float farPlane)
    : Camera(fovDegrees, aspect, nearPlane, farPlane), SceneObject() { }

void MovableCamera::update(float dt) {
    if (isLocked) return;

    glm::vec2 mouseDelta = InputManager::instance().getMouseDelta();
    yaw += mouseDelta.x * sensitivity;
    pitch -= mouseDelta.y * sensitivity;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    if (InputManager::instance().isKeyPressed(GLFW_KEY_W)) {
        this->position += speed * front * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_S)) {
        this->position -= speed * front * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_A)) {
        this->position -= speed * right * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_D)) {
        this->position += speed * right * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        this->position -= speed * up * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_SPACE)) {
        this->position += speed * up * dt;
    }

    this->target = this->position + glm::normalize(front);
}