#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class window {
public:
    window(int width, int height, const char* title);
    ~window();

    void clear_screen();
    void swap_buffers();
    void update();
    bool should_close() const;
    
private:
    GLFWwindow* handle;
};