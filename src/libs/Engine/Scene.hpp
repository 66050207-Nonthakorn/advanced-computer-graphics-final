#pragma once

#include "Engine/SceneObject.hpp"
#include "Engine/Light/DirectionalLight.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Camera.hpp"
#include "Engine/HDRCubemap.hpp"
#include "Engine/Texture/TextureManager.hpp"

#include <vector>
#include <memory>

struct Scene {
    Camera camera;

    std::vector<std::shared_ptr<SceneObject>> sceneObjects;
    
    std::vector<PointLight> lights;
    DirectionalLight directionalLight;
    
    HDRCubemap cubemap;

    virtual void update(float dt) = 0;
};