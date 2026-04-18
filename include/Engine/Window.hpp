#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>
#include "glm/vec2.hpp"

class Window {
public:
    Window(const int width, const int height, const std::string& title);
    ~Window();

    void swapBuffers();
    void pollEvent();
    bool shouldClose() const;
    void close();

    void setIsShowCursor(bool isShowCursor);

    glm::vec2 getMouseInput() const;
    bool isKeyPressed(int key) const;
    bool isKeyPressedOnce(int key);
    
private:
    GLFWwindow* handle;
};