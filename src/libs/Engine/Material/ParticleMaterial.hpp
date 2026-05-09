#pragma once

#include "Engine/Material/Material.hpp"

class ParticleMaterial : public Material {
public:
    ParticleMaterial();

    void bindPerFrame(const Material::PerFrameContext& context) override;
    void bindPerObject(const Material::PerObjectContext& context) override;
    bool isTransparent() const override { return true; }
};
