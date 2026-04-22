#include "Engine/Manager/InputManager.hpp"

InputManager& InputManager::instance() {
    static InputManager inst;
    return inst;
}

void InputManager::init(GLFWwindow* window) {
    handle = window;
    lastMouseX = -1;
    lastMouseY = -1;
}

glm::vec2 InputManager::getMouseDelta() {
    if (!handle) return {0, 0};

    double x, y;
    glfwGetCursorPos(handle, &x, &y);

    if (lastMouseX == -1 && lastMouseY == -1) {
        lastMouseX = x;
        lastMouseY = y;
    }

    glm::vec2 delta(x - lastMouseX, y - lastMouseY);
    lastMouseX = x;
    lastMouseY = y;
    return delta;
}

bool InputManager::isKeyPressed(int key) const {
    if (!handle) return false;
    return glfwGetKey(handle, key) == GLFW_PRESS;
}

bool InputManager::isKeyPressedOnce(int key) {
    if (!handle || key < 0 || key >= 512) return false;

    int state = glfwGetKey(handle, key);

    if (state == GLFW_PRESS) {
        prevKeyState[key] = true;
        return false;
    }

    if (state == GLFW_RELEASE && prevKeyState[key]) {
        prevKeyState[key] = false;
        return true;
    }

    return false;
}
