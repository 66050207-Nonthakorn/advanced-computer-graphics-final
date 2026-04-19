#pragma once

#include "Engine/Shader/Shader.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Light.hpp"

#include <memory>

struct MaterialPerFrameContext {
    const Camera& camera;
    const std::vector<Light>& lights;
};

struct MaterialPerObjectContext {
    Transform& transform;
};

class Material {
public:
    Material(const std::shared_ptr<Shader>& shader);
    virtual void bindPerFrame(const MaterialPerFrameContext& context) = 0;
    virtual void bindPerObject(const MaterialPerObjectContext& context) = 0;
protected:
    std::shared_ptr<Shader> shader;
};