#include "Engine/HDRCubemap.hpp"
#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Manager/ShaderManager.hpp"
#include "Engine/Texture/HDRCubemapTexture.hpp"

#include "GL/glew.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <array>
#include <cstdlib>
#include <iostream>

static void checkFramebuffer(const char* passName) {
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[HDRCubemap] Framebuffer incomplete during " << passName << "\n";
        std::exit(1);
    }
}

HDRCubemap::HDRCubemap()
    : cube(MeshManager::instance().get("cube"))
{}

HDRCubemap::HDRCubemap(
    const std::shared_ptr<Texture>& equirectangularTexture,
    const std::shared_ptr<Texture>& brdfLUT,
    int environmentSize,
    int irradianceSize,
    int prefilterSize,
    int prefilterMips
)
    : cube(MeshManager::instance().get("cube"))
{
    GLint previousViewport[4];
    glGetIntegerv(GL_VIEWPORT, previousViewport);

    GLuint fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const CaptureViews views {
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    bakeEnvironment(fbo, rbo, equirectangularTexture, environmentSize, prefilterMips, projection, views);
    bakeIrradiance(fbo, rbo, irradianceSize, projection, views);
    bakePrefilter(fbo, rbo, prefilterSize, prefilterMips, projection, views);

    iblBrdfLUT = brdfLUT;
    iblPrefilterMips = prefilterMips;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);

    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
}

void HDRCubemap::renderCube() const {
    glBindVertexArray(cube->vao);
    glDrawElements(GL_TRIANGLES, cube->indexCount, GL_UNSIGNED_INT, 0);
}

void HDRCubemap::captureFaces(
    unsigned int fbo,
    unsigned int rbo,
    const std::shared_ptr<Texture>& target,
    int mipLevel,
    int faceSize,
    const glm::mat4& projection,
    const CaptureViews& views,
    const std::shared_ptr<Shader>& shader
) const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, faceSize, faceSize);

    glViewport(0, 0, faceSize, faceSize);
    shader->use();
    shader->uniformMat4("projection", projection);

    for (int face = 0; face < 6; ++face) {
        shader->uniformMat4("view", views[face]);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
            target->getId(),
            mipLevel
        );
        checkFramebuffer("face capture");
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
}

void HDRCubemap::bakeEnvironment(
    unsigned int fbo, unsigned int rbo,
    const std::shared_ptr<Texture>& equirectangularTexture,
    int environmentSize, int prefilterMips,
    const glm::mat4& projection, const CaptureViews& views
) {
    const auto shader = ShaderManager::instance().get("cubemap-equirect");
    environmentMap = std::make_shared<HDRCubemapTexture>(environmentSize, std::max(1, prefilterMips));

    equirectangularTexture->bind(0);
    shader->use();
    shader->uniformInt("equirectangularMap", 0);
    captureFaces(fbo, rbo, environmentMap, 0, environmentSize, projection, views, shader);

    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap->getId());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void HDRCubemap::bakeIrradiance(
    unsigned int fbo, unsigned int rbo,
    int irradianceSize,
    const glm::mat4& projection, const CaptureViews& views
) {
    const auto shader = ShaderManager::instance().get("cubemap-irradiance");
    iblIrradianceMap = std::make_shared<HDRCubemapTexture>(irradianceSize);

    environmentMap->bind(0);
    shader->use();
    shader->uniformInt("environmentMap", 0);
    captureFaces(fbo, rbo, iblIrradianceMap, 0, irradianceSize, projection, views, shader);
}

void HDRCubemap::bakePrefilter(
    unsigned int fbo, unsigned int rbo,
    int prefilterSize, int prefilterMips,
    const glm::mat4& projection, const CaptureViews& views
) {
    const auto shader = ShaderManager::instance().get("cubemap-prefilter");
    iblPrefilterMap = std::make_shared<HDRCubemapTexture>(prefilterSize, prefilterMips);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap->getId());
    shader->use();
    shader->uniformInt("environmentMap", 0);

    for (int mip = 0; mip < prefilterMips; ++mip) {
        const int mipSize = std::max(1, prefilterSize >> mip);
        const float roughness = prefilterMips > 1
            ? static_cast<float>(mip) / static_cast<float>(prefilterMips - 1)
            : 0.0f;
        shader->uniformFloat("roughness", roughness);
        captureFaces(fbo, rbo, iblPrefilterMap, mip, mipSize, projection, views, shader);
    }
}
