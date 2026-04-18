#include "Engine/Window.hpp"
#include <iostream>

Window::Window(const int width, const int height, const std::string& title) {
    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "GLFW init failed" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (!(handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr))) {
        std::cerr << "GLFW window creation failed" << std::endl;
        glfwTerminate();
        exit(1);
    }

    int buf_w, buf_h;
    glfwGetFramebufferSize(handle, &buf_w, &buf_h);
    glfwMakeContextCurrent(handle);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed" << std::endl;
        glfwDestroyWindow(handle);
        glfwTerminate();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, buf_w, buf_h);
}

Window::~Window() {
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void Window::pollEvent() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(this->handle);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(this->handle);
}

void Window::close() {
    glfwSetWindowShouldClose(this->handle, GLFW_TRUE);
}

void Window::setIsShowCursor(bool isShowCursor) {
    int mode = (isShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    glfwSetInputMode(this->handle, GLFW_CURSOR, mode);
}

glm::vec2 Window::getMouseInput() const {
    static double lastFrameX = -1;
    static double lastFrameY = -1;

    double x, y;
    glfwGetCursorPos(this->handle, &x, &y);

    if (lastFrameX == -1 && lastFrameY == -1) {
        lastFrameX = x;
        lastFrameY = y;
    }

    glm::vec2 delta(x - lastFrameX, y - lastFrameY);
    lastFrameX = x;
    lastFrameY = y;

    return delta;
}

bool Window::isKeyPressed(int key) const {
    return glfwGetKey(this->handle, key) == GLFW_PRESS;
}

bool Window::isKeyPressedOnce(int key) {
    static bool isPressedLastFrame[512];

    int keyState = glfwGetKey(this->handle, key);
    
    if (keyState == GLFW_PRESS) {
        isPressedLastFrame[key] = true;
        return false;
    }

    if (keyState == GLFW_RELEASE && isPressedLastFrame[key]) {
        isPressedLastFrame[key] = false;
        return true;
    }

    return false;
}
