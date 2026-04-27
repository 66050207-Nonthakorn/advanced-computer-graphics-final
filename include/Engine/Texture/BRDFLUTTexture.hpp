#pragma once

#include "Engine/Texture/Texture.hpp"
#include "GL/glew.h"
#include <string>

class BRDFLUTTexture : public Texture {
public:
    BRDFLUTTexture(int size);
    ~BRDFLUTTexture() override;

    int getType() const override { return GL_TEXTURE_2D; }
};
