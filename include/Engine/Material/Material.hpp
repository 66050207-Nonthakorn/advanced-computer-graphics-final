#pragma once

#include "Engine/Shader/Shader.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Light/DirectionalLight.hpp"

#include "glm/glm.hpp"
#include <memory>

class Material {
public:
    struct PerFrameContext {
        const Camera& camera;
        const std::vector<PointLight>& lights;
        const DirectionalLight& directionalLight;
        unsigned int dirShadowMap;
        glm::mat4 lightSpaceMatrix;
    };

    struct PerObjectContext {
        Transform& transform;
    };
    
    Material(const std::shared_ptr<Shader>& shader);
    virtual void bindPerFrame(const Material::PerFrameContext& context) = 0;
    virtual void bindPerObject(const Material::PerObjectContext& context) = 0;
    virtual ~Material() = default;
    
protected:
    std::shared_ptr<Shader> shader;
};