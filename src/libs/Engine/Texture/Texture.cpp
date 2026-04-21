#include "Engine/Texture/Texture.hpp"
#include "GL/glew.h"
#include "stb_image.h"

#include <iostream>

static void uploadToGPU(unsigned int& id, unsigned char* data, int width, int height, int nrChannels, int format) {
    int pixelFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    int internalFormat = format;
    if (nrChannels == 4) {
        if (format == GL_SRGB) internalFormat = GL_SRGB_ALPHA;
        else                   internalFormat = GL_RGBA;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(const std::string &filePath, const int format, const bool isFlip) {
    stbi_set_flip_vertically_on_load(isFlip);

    stbi_uc* data = stbi_load(filePath.c_str(), &this->width, &this->height, &this->nrChannels, 0);
    if (!data) {
        std::cerr << "[Texture] failed to load image from " << filePath << std::endl;
        exit(1);
    }

    uploadToGPU(this->id, data, this->width, this->height, this->nrChannels, format);
    stbi_image_free(data);
}

Texture::Texture(unsigned char* data, int width, int height, int nrChannels, const int format)
    : width(width), height(height), nrChannels(nrChannels)
{
    uploadToGPU(this->id, data, width, height, nrChannels, format);
}

Texture::~Texture() {
    glDeleteTextures(1, &this->id);
}