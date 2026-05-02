#pragma once

#include "GL/glew.h"

#include "Texture.hpp"
#include <string>

class HDREquirectangularTexture : public Texture {
public:
    HDREquirectangularTexture(const std::string& path);
    int getType() const override { return GL_TEXTURE_2D; }
};