#pragma once

#include "Material.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Texture/Texture.hpp"

#include <memory>

class BlinnPhongMaterial : public Material {
public:
    BlinnPhongMaterial();
    BlinnPhongMaterial(const std::shared_ptr<Texture>& texture);
    
    void bindPerFrame(const MaterialPerFrameContext& context) override;
    void bindPerObject(const MaterialPerObjectContext& context) override;

private:
    std::shared_ptr<Texture> texture;
};