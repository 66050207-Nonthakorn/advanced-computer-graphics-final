#pragma once

#include "Engine/SceneObject.hpp"
#include "Engine/Light.hpp"

#include <vector>
#include <memory>

struct Scene {
    std::vector<SceneObject> sceneObjects;
    std::vector<Light> lights;

    virtual void update(float dt) = 0;
};