#include "Engine/Cubemap.hpp"
#include "Engine/Mesh/MeshManager.hpp"

#include "gl/glew.h"
#include "stb_image.h"

#include <array>
#include <iostream>
#include <string>

Cubemap::Cubemap(std::shared_ptr<Texture> texture)
    : cube(MeshManager::instance().get("cube"))
{
    if (auto cubemapTexture = std::static_pointer_cast<CubemapTexture>(texture)) {
        this->texture = cubemapTexture;
    }
    else {
        std::cerr << "[Cubemap] texture is not cubemap texture" << std::endl;
        exit(1);
    }
}