#pragma once

#include "Material.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Texture/Texture.hpp"

#include <memory>

class BlinnPhongMaterial : public Material {
public:
    BlinnPhongMaterial();
    BlinnPhongMaterial(const std::shared_ptr<Texture>& texture);
    
    void bindPerFrame(const Material::PerFrameContext& context) override;
    void bindPerObject(const Material::PerObjectContext& context) override;

private:
    std::shared_ptr<Texture> texture;
    float ambient, diffuse, specular, shininess;
};