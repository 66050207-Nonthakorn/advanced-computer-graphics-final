#pragma once

#include "Material.hpp"
#include "Engine/Texture/Texture.hpp"
#include "glm/glm.hpp"

class PBRMaterial : public Material {
public:
    PBRMaterial(
        const std::shared_ptr<Texture>& albedo,
        const std::shared_ptr<Texture>& ao,
        const std::shared_ptr<Texture>& metallic,
        const std::shared_ptr<Texture>& normal,
        const std::shared_ptr<Texture>& roughness
    );

    void bindPerFrame(const Material::PerFrameContext& context) override;
    void bindPerObject(const Material::PerObjectContext& context) override;

    float normalStrength = 1.0f;
    float metallicValue  = 0.0f;
    float roughnessValue = 0.7f;
    glm::vec3 albedoColor = glm::vec3(1.0f); // used when no albedo texture
    // 0=off, 1=shadow only, 2=ambient only, 3=diffuse only, 4=specular only
    int debugMode = 0;

protected:
    std::shared_ptr<Texture> albedo, ao, metallic, normal, roughness;
};