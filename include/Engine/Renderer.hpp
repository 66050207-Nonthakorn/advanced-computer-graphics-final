#pragma once

#include "Mesh/Mesh.hpp"
#include "SceneObject.hpp"
#include "Camera.hpp"
#include "Engine/Scene.hpp"

#include <vector>

class Renderer {
public:
    void clearScreen();
    void draw(
        Scene& scene,
        const Camera& camera
    );
};