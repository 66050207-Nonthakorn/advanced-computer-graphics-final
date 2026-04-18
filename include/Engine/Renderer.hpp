#pragma once

#include "Mesh/Mesh.hpp"
#include "SceneObject.hpp"
#include "Camera.hpp"

#include <vector>

class Renderer {
public:
    void clearScreen();
    void draw(
        std::vector<std::shared_ptr<SceneObject>>& sceneObjects,
        const Camera& camera
    );
};