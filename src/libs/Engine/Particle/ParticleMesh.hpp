#pragma once

#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Vertex.hpp"
#include "Engine/Particle/Particle.hpp"

#include "glm/glm.hpp"

#include <vector>

class ParticleMesh : public Mesh {
public:
    explicit ParticleMesh(int maxParticles = 1000);

    void update(float dt, const glm::mat4& viewMatrix);

    // Emitter settings
    glm::vec3 emitterPosition = { 0.0f,  0.0f, 0.0f };
    glm::vec3 gravity         = { 0.0f, -4.0f, 0.0f };
    float     emitRate        = 50.0f;
    float     minLife         = 1.0f;
    float     maxLife         = 2.5f;
    float     minSize         = 0.04f;
    float     maxSize         = 0.12f;
    glm::vec3 minVelocity     = { -1.0f, 2.0f, -1.0f };
    glm::vec3 maxVelocity     = {  1.0f, 4.0f,  1.0f };
    glm::vec4 startColor      = { 1.0f, 0.6f, 0.1f, 1.0f };
    glm::vec4 endColor        = { 0.8f, 0.1f, 0.0f, 0.0f };

    int activeCount() const { return static_cast<int>(particles.size()); }

private:
    int   maxParticles;
    float emitAccum = 0.0f;

    std::vector<Particle>      particles;
    std::vector<Vertex>        cpuVertices;
    std::vector<unsigned int>  cpuIndices;

    void rebuildQuads(const glm::mat4& viewMatrix);
};
