#pragma once

#include "Texture.hpp"
#include "GL/glew.h"
#include <array>
#include <string>
#include <vector>

// Single-level HDR cubemap (irradiance map)
// Multi-level HDR cubemap with mipmaps (prefiltered environment map)
class HDRCubemapTexture : public Texture {
public:
    // Single mip level — use for irradiance map
    HDRCubemapTexture(const std::array<std::string, 6>& paths);

    // Multiple mip levels — use for prefiltered specular map.
    // mipPaths[i] holds the 6 face paths for mip level i.
    HDRCubemapTexture(const std::vector<std::array<std::string, 6>>& mipPaths);

    ~HDRCubemapTexture() override;
    int getType() const override { return GL_TEXTURE_CUBE_MAP; }
};
