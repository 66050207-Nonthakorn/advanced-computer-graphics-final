#include "Engine/Texture/Texture.hpp"
#include "GL/glew.h"
#include "stb_image.h"

#include <iostream>

Texture::Texture(const std::string &filePath) {
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* data = stbi_load(filePath.c_str(), &this->width, &this->height, &this->nrChannels, 0);
    if (!data) {
        std::cerr << "[Texture] failed to load image from " << filePath << std::endl;
        exit(1);
    }

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}