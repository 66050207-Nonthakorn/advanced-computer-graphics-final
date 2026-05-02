#include "Engine/Texture/HDRCubemapTexture.hpp"

#include "stb_image.h"
#include <algorithm>
#include <iostream>

HDRCubemapTexture::HDRCubemapTexture(int size, int mipLevels) {
    const int totalMips = std::max(1, mipLevels);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int mip = 0; mip < totalMips; ++mip) {
        const int mipSize = std::max(1, size >> mip);
        for (int face = 0; face < 6; ++face) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                mip,
                GL_RGB16F,
                mipSize,
                mipSize,
                0,
                GL_RGB,
                GL_FLOAT,
                nullptr
            );
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, totalMips - 1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MIN_FILTER,
        totalMips > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR
    );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

HDRCubemapTexture::~HDRCubemapTexture() {
    glDeleteTextures(1, &id);
}
