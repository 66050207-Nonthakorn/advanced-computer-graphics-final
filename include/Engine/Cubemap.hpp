#pragma once

#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Texture/CubemapTexture.hpp"
#include <memory>

class Cubemap {
public:
    Cubemap(std::shared_ptr<Texture> texture);

    std::shared_ptr<Mesh> cube;
    std::shared_ptr<CubemapTexture> texture;
    unsigned int id;
};