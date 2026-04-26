#include "Engine/Texture/CubemapTexture.hpp"

#include "stb_image.h"
#include <iostream>

CubemapTexture::CubemapTexture(const std::array<std::string, 6>& paths) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    stbi_set_flip_vertically_on_load(false);

    for (size_t faceIndex = 0; faceIndex < paths.size(); ++faceIndex) {
        int width = 0;
        int height = 0;
        int channelCount = 0;
        stbi_uc* data = stbi_load(paths[faceIndex].c_str(), &width, &height, &channelCount, 0);

        if (!data) {
            std::cerr << "[Cubemap] failed to load image from " << paths[faceIndex];
            const char* reason = stbi_failure_reason();
            if (reason) {
                std::cerr << " (" << reason << ")";
            }
            std::cerr << std::endl;
            exit(1);
        }

        int pixelFormat;
        switch (channelCount) {
            case 1: pixelFormat = GL_RED; break;
            case 2: pixelFormat = GL_RG; break;
            case 3: pixelFormat = GL_RGB; break;
            case 4: pixelFormat = GL_RGBA; break;
            default:
                std::cerr << "[CubemapTexture] unsupported channel count " << channelCount << std::endl;
                exit(1);
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(faceIndex),
            0,
            pixelFormat,
            width,
            height,
            0,
            pixelFormat,
            GL_UNSIGNED_BYTE,
            data
        );
        stbi_image_free(data);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

CubemapTexture::~CubemapTexture() {

}
