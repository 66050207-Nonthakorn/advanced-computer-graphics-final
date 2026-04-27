#include "Engine/Texture/HDRCubemapTexture.hpp"

#include "stb_image.h"
#include <iostream>

// Loads a single HDR face into the bound cubemap at the given mip level.
// Returns false and prints an error if loading fails.
static bool loadHDRFace(GLenum faceTarget, int mipLevel, const std::string& path) {
    stbi_set_flip_vertically_on_load(false);

    int width = 0, height = 0, channels = 0;
    float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "[HDRCubemapTexture] Failed to load: " << path;
        const char* reason = stbi_failure_reason();
        if (reason) std::cerr << " (" << reason << ")";
        std::cerr << std::endl;
        return false;
    }

    GLenum internalFormat = GL_RGB16F;
    GLenum format = GL_RGB;
    if (channels == 4) {
        internalFormat = GL_RGBA16F;
        format = GL_RGBA;
    }

    glTexImage2D(faceTarget, mipLevel, internalFormat, width, height, 0, format, GL_FLOAT, data);
    stbi_image_free(data);
    return true;
}

HDRCubemapTexture::HDRCubemapTexture(const std::array<std::string, 6>& paths) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int i = 0; i < 6; ++i) {
        loadHDRFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, paths[i]);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

HDRCubemapTexture::HDRCubemapTexture(const std::vector<std::array<std::string, 6>>& mipPaths) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    for (int mip = 0; mip < static_cast<int>(mipPaths.size()); ++mip) {
        for (int face = 0; face < 6; ++face) {
            loadHDRFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, mipPaths[mip][face]);
        }
    }

    int maxMip = static_cast<int>(mipPaths.size()) - 1;
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, maxMip);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

HDRCubemapTexture::~HDRCubemapTexture() {
    glDeleteTextures(1, &id);
}
