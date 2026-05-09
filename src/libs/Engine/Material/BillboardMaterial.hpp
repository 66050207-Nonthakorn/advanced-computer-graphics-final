#pragma once

#include "Material.hpp"
#include "Engine/Texture/Texture.hpp"

#include "glm/vec4.hpp"
#include <memory>

class BillboardMaterial : public Material {
public:
    BillboardMaterial(const glm::vec4& color = glm::vec4(1.0f));
    BillboardMaterial(const std::shared_ptr<Texture>& texture, const glm::vec4& color = glm::vec4(1.0f));

    void bindPerFrame(const Material::PerFrameContext& context) override;
    void bindPerObject(const Material::PerObjectContext& context) override;
    bool isTransparent() const override { return color.a < 1.0f; }
    bool isOverlay() const override { return true; }

    glm::vec4 color;

private:
    std::shared_ptr<Texture> texture;
};
