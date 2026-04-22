#pragma once

#include "Engine/SceneObject.hpp"
#include "Engine/Light/DirectionalLight.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Camera.hpp"

#include <vector>
#include <memory>

struct Scene {
    Camera camera;
    std::vector<SceneObject> sceneObjects;
    std::vector<PointLight> lights;
    DirectionalLight directionalLight;

    virtual void update(float dt) = 0;
};