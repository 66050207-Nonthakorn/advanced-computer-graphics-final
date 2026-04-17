#include "window.hpp"
#include <iostream>

window::window(int width, int height, const char *title) {
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

    if (!(handle = glfwCreateWindow(width, height, "Advanced CG Lab", nullptr, nullptr))) {
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

    glViewport(0, 0, buf_w, buf_h);
}

window::~window() {
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void window::clear_screen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void window::update() {
    glfwSwapBuffers(this->handle);
    glfwPollEvents();
}

void window::swap_buffers() {
    glfwSwapBuffers(this->handle);
}

bool window::should_close() const {
    return glfwWindowShouldClose(this->handle);
}
