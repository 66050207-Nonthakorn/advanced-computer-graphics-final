#pragma once

#include "Engine/Shader/Shader.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Light.hpp"

#include <memory>
class Material {
public:
    struct PerFrameContext {
        const Camera& camera;
        const std::vector<Light>& lights;
    };

    struct PerObjectContext {
        Transform& transform;
    };
    
    Material(const std::shared_ptr<Shader>& shader);
    virtual void bindPerFrame(const Material::PerFrameContext& context) = 0;
    virtual void bindPerObject(const Material::PerObjectContext& context) = 0;
protected:
    std::shared_ptr<Shader> shader;
};