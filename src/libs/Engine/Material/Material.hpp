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
        
        // IBL — zero means not available
        unsigned int iblIrradianceMap = 0;
        unsigned int iblPrefilterMap = 0;
        int iblPrefilterMips = 0;
        unsigned int iblBrdfLUT = 0;
    };

    struct PerObjectContext {
        glm::mat4 modelMatrix;
    };
    
    Material(const std::shared_ptr<Shader>& shader);
    virtual void bindPerFrame(const Material::PerFrameContext& context) = 0;
    virtual void bindPerObject(const Material::PerObjectContext& context) = 0;
    virtual bool isTransparent() const { return false; }
    virtual ~Material() = default;
    
protected:
    std::shared_ptr<Shader> shader;
};