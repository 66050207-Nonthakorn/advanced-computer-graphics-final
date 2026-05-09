#pragma once

#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Shader/Shader.hpp"
#include "Engine/Texture/Texture.hpp"

#include "glm/mat4x4.hpp"

#include <array>
#include <memory>

class HDRCubemap {
public:
    HDRCubemap();
    HDRCubemap(
        const std::shared_ptr<Texture>& equirectangularTexture,
        const std::shared_ptr<Texture>& brdfLUT,
        int environmentSize = 1024,
        int irradianceSize = 32,
        int prefilterSize = 128,
        int prefilterMips = 5
    );

    std::shared_ptr<Texture> environmentMap;
    std::shared_ptr<Texture> iblIrradianceMap;
    std::shared_ptr<Texture> iblPrefilterMap;
    std::shared_ptr<Texture> iblBrdfLUT;
    int iblPrefilterMips = 0;
    float iblIntensity = 0;

    std::shared_ptr<Mesh> cube;
    unsigned int id;

private:
    using CaptureViews = std::array<glm::mat4, 6>;

    void renderCube() const;
    void captureFaces(
        unsigned int fbo,
        unsigned int rbo,
        const std::shared_ptr<Texture>& target,
        int mipLevel,
        int faceSize,
        const glm::mat4& projection,
        const CaptureViews& views,
        const std::shared_ptr<Shader>& shader
    ) const;

    void bakeEnvironment(
        unsigned int fbo, unsigned int rbo,
        const std::shared_ptr<Texture>& equirectangularTexture,
        int environmentSize, int prefilterMips,
        const glm::mat4& projection, const CaptureViews& views
    );
    void bakeIrradiance(
        unsigned int fbo, unsigned int rbo,
        int irradianceSize,
        const glm::mat4& projection, const CaptureViews& views
    );
    void bakePrefilter(
        unsigned int fbo, unsigned int rbo,
        int prefilterSize, int prefilterMips,
        const glm::mat4& projection, const CaptureViews& views
    );
};
