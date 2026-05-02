#pragma once

#include "SceneObject.hpp"
#include "Camera.hpp"

#include "Scene.hpp"
#include "Mesh/Mesh.hpp"
#include "Window.hpp"

#include <vector>

class Renderer {
public:
    Renderer(const Window& window);

    void clearScreen();
    void draw(Scene& scene);

private:
    const Window& window;
};