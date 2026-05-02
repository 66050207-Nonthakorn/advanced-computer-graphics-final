#pragma once

#include "Texture.hpp"
#include "GL/glew.h"
#include <array>
#include <string>
#include <vector>

class HDRCubemapTexture : public Texture {
public:
    HDRCubemapTexture(int size, int mipLevels = 1);

    ~HDRCubemapTexture() override;
    int getType() const override { return GL_TEXTURE_CUBE_MAP; }
};
