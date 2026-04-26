#pragma once

#include "Engine/SceneObject.hpp"
#include "Engine/Light/DirectionalLight.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Cubemap.hpp"
#include "Engine/Texture/TextureManager.hpp"

#include <vector>
#include <memory>

struct Scene {
    Camera camera;

    std::vector<SceneObject> sceneObjects;
    
    std::vector<PointLight> lights;
    DirectionalLight directionalLight;
    
    std::unique_ptr<Cubemap> cubemap = std::make_unique<Cubemap>(TextureManager::instance().get("room-cubemap"));

    virtual void update(float dt) = 0;
};