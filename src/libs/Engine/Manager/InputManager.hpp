#pragma once

#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

class InputManager {
public:
    static InputManager& instance();

    InputManager(InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    void init(GLFWwindow* window);

    glm::vec2 getMouseDelta();
    bool isKeyPressed(int key) const;
    bool isKeyPressedOnce(int key);

private:
    InputManager() = default;

    GLFWwindow* handle = nullptr;

    double lastMouseX = -1;
    double lastMouseY = -1;
    bool prevKeyState[512] = {};
};