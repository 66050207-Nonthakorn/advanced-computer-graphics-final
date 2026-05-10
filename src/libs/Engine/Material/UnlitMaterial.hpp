#pragma once

#include "Engine/Material/Material.hpp"

class UnlitMaterial : public Material {
public:
    explicit UnlitMaterial(const glm::vec4& color = glm::vec4(1.0f));

    void bindPerFrame(const Material::PerFrameContext& context) override;
    void bindPerObject(const Material::PerObjectContext& context) override;

private:
    glm::vec4 color;
};
