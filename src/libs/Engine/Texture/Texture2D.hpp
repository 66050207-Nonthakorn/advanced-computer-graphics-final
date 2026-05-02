#pragma once

#include "Engine/Texture/Texture.hpp"
#include "GL/glew.h"

class Texture2D : public Texture {
public:
    Texture2D(
        const std::string& path,
        bool isSRGB = true,
        bool isFlip = true
    );

    ~Texture2D() override;
    int getType() const override { return GL_TEXTURE_2D; }
    
private:
    int width, height, nrChannels, internalFormat;
};