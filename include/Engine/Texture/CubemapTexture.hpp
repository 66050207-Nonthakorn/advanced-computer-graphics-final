#pragma once

#include "Texture.hpp"
#include "GL/glew.h"
#include <array>

class CubemapTexture : public Texture {
public:
    CubemapTexture(const std::array<std::string, 6>& paths);
    ~CubemapTexture() override;
    int getType() const override { return GL_TEXTURE_CUBE_MAP; }
};