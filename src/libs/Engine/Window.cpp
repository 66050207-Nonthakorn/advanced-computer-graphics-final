#include "Engine/Window.hpp"
#include <iostream>
#include "Window.hpp"

Window::Window(const int width, const int height, const std::string& title): width(width), height(height) {
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
    glfwWindowHint(GLFW_SAMPLES, 4);

    if (!(handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr))) {
        std::cerr << "GLFW window creation failed" << std::endl;
        glfwTerminate();
        exit(1);
    }

    int buf_w, buf_h;
    glfwGetFramebufferSize(handle, &buf_w, &buf_h);
    glfwMakeContextCurrent(handle);
    glewExperimental = GL_TRUE;

    // glfwSwapInterval(0); // Disable V-Sync

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed" << std::endl;
        glfwDestroyWindow(handle);
        glfwTerminate();
        exit(1);
    }

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
    this->isShowCursor = isShowCursor;
    int mode = (isShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    glfwSetInputMode(this->handle, GLFW_CURSOR, mode);
}

GLFWwindow* Window::getHandle() const {
    return handle;
}
