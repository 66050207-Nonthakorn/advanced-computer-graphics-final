#pragma once

#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Texture/Texture.hpp"
#include <memory>

class HDRCubemap {
public:
    HDRCubemap();

    std::shared_ptr<Texture> iblIrradianceMap;
    std::shared_ptr<Texture> iblPrefilterMap;
    std::shared_ptr<Texture> iblBrdfLUT;
    int iblPrefilterMips = 0;

    std::shared_ptr<Mesh> cube;
    unsigned int id;
};