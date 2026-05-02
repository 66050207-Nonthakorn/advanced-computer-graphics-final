#pragma once

#include "glm/vec3.hpp"
#include "glm/glm.hpp"

static const unsigned int SHADOW_MAP_SIZE = 4096;

class DirectionalLight {
public:
    DirectionalLight();

    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    glm::mat4 getLightSpaceMatrix() const;
    unsigned int getDepthMapID() const;
    unsigned int getShadowFBO() const;
private:
    unsigned int depthMap, shadowFBO;
};