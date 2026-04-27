#include "Engine/Texture/Texture.hpp"
#include "GL/glew.h"
#include "stb_image.h"

#include <iostream>

void Texture::bind(int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(this->getType(), this->id);
}

unsigned int Texture::getId() const {
    return id;
}