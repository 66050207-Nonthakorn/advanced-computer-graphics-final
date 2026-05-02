#include "HDREquirectangularTexture.hpp"

#include "stb_image.h"
#include <iostream>

HDREquirectangularTexture::HDREquirectangularTexture(const std::string &path) {
    stbi_set_flip_vertically_on_load(true);

    int width = 0;
    int height = 0;
    int channels = 0;
    float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "[HDREquirectangularTexture] Failed to load equirectangular HDR from " << path;
        const char* reason = stbi_failure_reason();
        if (reason) {
            std::cerr << " (" << reason << ")";
        }
        std::cerr << std::endl;
        std::exit(1);
    }

    GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
    GLenum internalFormat = channels == 4 ? GL_RGBA16F : GL_RGB16F;

    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}