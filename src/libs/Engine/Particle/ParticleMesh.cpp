#include "Engine/Particle/ParticleMesh.hpp"
#include "Engine/Utils/RandomUtils.hpp"

#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"

#include <algorithm>
#include <iostream>
#include <cstdlib>

static MeshData buildEmptyMesh(int maxParticles) {
    MeshData d;
    d.vertices.resize(maxParticles * 4);
    d.indices.resize(maxParticles * 6);
    return d;
}

ParticleMesh::ParticleMesh(int maxParticles)
    : Mesh(buildEmptyMesh(maxParticles)), maxParticles(maxParticles)
{
    cpuVertices.resize(maxParticles * 4);
    cpuIndices.resize(maxParticles * 6);

    // Pre-fill index buffer
    for (int i = 0; i < maxParticles; ++i) {
        unsigned int base = i * 4;
        int          off  = i * 6;
        cpuIndices[off + 0] = base + 0;
        cpuIndices[off + 1] = base + 1;
        cpuIndices[off + 2] = base + 2;
        cpuIndices[off + 3] = base + 0;
        cpuIndices[off + 4] = base + 2;
        cpuIndices[off + 5] = base + 3;
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        maxParticles * 6 * sizeof(unsigned int),
        cpuIndices.data(),
        GL_DYNAMIC_DRAW
    );
    glBindVertexArray(0);

    particles.reserve(maxParticles);
    indexCount = 0;
}

void ParticleMesh::update(float dt, const glm::mat4& viewMatrix) {
    // Auto-emit
    emitAccum += emitRate * dt;
    while (emitAccum >= 1.0f && static_cast<int>(particles.size()) < maxParticles) {
        Particle p;
        p.position = emitterPosition;
        p.velocity = { RandomUtils::randRange(minVelocity.x, maxVelocity.x),
                        RandomUtils::randRange(minVelocity.y, maxVelocity.y),
                        RandomUtils::randRange(minVelocity.z, maxVelocity.z) };
        p.maxLife  = RandomUtils::randRange(minLife, maxLife);
        p.life     = p.maxLife;
        p.size     = RandomUtils::randRange(minSize, maxSize);
        p.color    = startColor;
        particles.push_back(p);
        emitAccum -= 1.0f;
    }

    // Simulate
    for (auto& p : particles) {
        p.velocity += gravity * dt;
        p.position += p.velocity * dt;
        p.life     -= dt;

        float t  = 1.0f - (p.life / p.maxLife); // 0 → 1 over lifetime
        p.color   = glm::mix(startColor, endColor, t);
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p) { return p.life <= 0.0f; }),
        particles.end()
    );

    rebuildQuads(viewMatrix);
}

void ParticleMesh::rebuildQuads(const glm::mat4& viewMatrix) {
    // Camera right/up extracted from the view matrix rows
    glm::vec3 right = { viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0] };
    glm::vec3 up    = { viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1] };

    int n = static_cast<int>(particles.size());

    for (int i = 0; i < n; i++) {
        const Particle& p = particles[i];
        float lifeT = 1.0f - (p.life / p.maxLife);
        float s = p.size * (0.65f + 1.6f * lifeT);

        glm::vec3 bl = p.position - right * s - up * s;
        glm::vec3 br = p.position + right * s - up * s;
        glm::vec3 tr = p.position + right * s + up * s;
        glm::vec3 tl = p.position - right * s + up * s;

        float brightness = glm::dot(glm::vec3(p.color), glm::vec3(0.299f, 0.587f, 0.114f));
        float alpha = p.color.a;

        int base = i * 4;
        cpuVertices[base + 0] = { bl, { brightness, 0.0f, alpha }, { 0.0f, 0.0f } };
        cpuVertices[base + 1] = { br, { brightness, 0.0f, alpha }, { 1.0f, 0.0f } };
        cpuVertices[base + 2] = { tr, { brightness, 1.0f, alpha }, { 1.0f, 1.0f } };
        cpuVertices[base + 3] = { tl, { brightness, 1.0f, alpha }, { 0.0f, 1.0f } };
    }

    indexCount = static_cast<size_t>(n) * 6;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, n * 4 * sizeof(Vertex), cpuVertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
