#include "Engine/Texture/Texture2D.hpp"

#include "stb_image.h"
#include <iostream>

Texture2D::Texture2D(const std::string &path, bool isSRGB, bool isFlip) {
    stbi_set_flip_vertically_on_load(isFlip);

    stbi_uc* data = stbi_load(path.c_str(), &this->width, &this->height, &this->nrChannels, 0);
    if (!data) {
        std::cerr << "[Texture2D] failed to load image from " << path;
        const char* reason = stbi_failure_reason();
        if (reason) {
            std::cerr << " (" << reason << ")";
        }
        exit(1);
    }

    GLenum pixelFormat = GL_RGB;
    switch (nrChannels) {
        case 1: pixelFormat = GL_RED;  break;
        case 2: pixelFormat = GL_RG;   break;
        case 3: pixelFormat = GL_RGB;  break;
        case 4: pixelFormat = GL_RGBA; break;
        default:
            std::cerr << "[Texture2D] unsupported channel count " << nrChannels << std::endl;
            exit(1);
    }

    switch (pixelFormat) {
        case GL_RED:  internalFormat = GL_RED; break;
        case GL_RG:   internalFormat = GL_RG; break;
        case GL_RGB:  internalFormat = isSRGB ? GL_SRGB : GL_RGB; break;
        case GL_RGBA: internalFormat = isSRGB ? GL_SRGB_ALPHA : GL_RGBA; break;
        default:      internalFormat = GL_RGB; break;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    stbi_image_free(data);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &this->id);
}
