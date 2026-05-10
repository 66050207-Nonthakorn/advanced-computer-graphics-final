#include "Engine/Cloth/ClothMesh.hpp"

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

static MeshData buildInitialMeshData(int rows, int cols, float width, float height) {
    MeshData data;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float u = static_cast<float>(c) / (cols - 1);
            float v = static_cast<float>(r) / (rows - 1);
            glm::vec3 pos = { (0.5f - u) * width, (0.5f - v) * height, 0.0f };
            data.vertices.push_back({ pos, {0, 0, 1}, {u, v} });
        }
    }

    for (int r = 0; r < rows - 1; ++r) {
        for (int c = 0; c < cols - 1; ++c) {
            unsigned int i = r * cols + c;
            data.indices.insert(data.indices.end(), {
                i,          i + 1,      i + cols,
                i + 1,      i + cols + 1, i + cols
            });
        }
    }

    return data;
}

ClothMesh::ClothMesh(int rows, int cols, float width, float height)
    : Mesh(buildInitialMeshData(rows, cols, width, height)),
      rows(rows), cols(cols)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 rows * cols * sizeof(Vertex),
                 nullptr,
                 GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    buildGrid(width, height);
    recalcNormals();
    uploadToGPU();
}

void ClothMesh::buildGrid(float width, float height) {
    particles.resize(rows * cols);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float u = static_cast<float>(c) / (cols - 1);
            float v = static_cast<float>(r) / (rows - 1);
            glm::vec3 pos = { (0.5f - u) * width, (0.5f - v) * height, 0.0f };

            auto& p = particles[r * cols + c];
            p.position     = pos;
            p.prevPosition = pos;
            p.normal       = { 0, 0, 1 };
            p.uv           = { u, v };
        }
    }

    cpuVertices.resize(rows * cols);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (c + 1 < cols) addSpring(r * cols + c, r * cols + c + 1);
            if (r + 1 < rows) addSpring(r * cols + c, (r + 1) * cols + c);
        }
    }

    for (int r = 0; r < rows - 1; ++r) {
        for (int c = 0; c < cols - 1; ++c) {
            addSpring(r * cols + c,         (r + 1) * cols + c + 1);
            addSpring(r * cols + c + 1,     (r + 1) * cols + c);
        }
    }

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols - 2; ++c) addSpring(r * cols + c, r * cols + c + 2);
    }
    for (int r = 0; r < rows - 2; ++r) {
        for (int c = 0; c < cols; ++c)     addSpring(r * cols + c, (r + 2) * cols + c);
    }
}

void ClothMesh::addSpring(int a, int b) {
    float len = glm::length(particles[a].position - particles[b].position);
    springs.push_back({ a, b, len });
}

void ClothMesh::pin(int index)   {
    if (index >= 0 && index < particles.size()) {
        particles[index].pinned = true;
    }
}

void ClothMesh::unpin(int index) {
    if (index >= 0 && index < particles.size()) {
        particles[index].pinned = false;
    }
}

void ClothMesh::applyWind(float dt) {
    if (wind == glm::vec3(0.0f) && windGustAmplitude == 0.0f) return;

    float gust = windGustAmplitude * (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f);
    glm::vec3 windDir = glm::normalize(wind);
    glm::vec3 windForce = wind + windDir * gust;

    float dt2 = dt * dt;

    for (int r = 0; r < rows - 1; ++r) {
        for (int c = 0; c < cols - 1; ++c) {
            int i00 = r * cols + c;
            int i10 = i00 + 1;
            int i01 = i00 + cols;
            int i11 = i00 + cols + 1;

            auto applyTri = [&](int a, int b, int c) {
                glm::vec3 edge1 = particles[b].position - particles[a].position;
                glm::vec3 edge2 = particles[c].position - particles[a].position;
                glm::vec3 faceNormal = glm::cross(edge1, edge2);
                float area = glm::length(faceNormal);
                if (area < 1e-6f) return;
                faceNormal /= area;

                float exposure = glm::dot(faceNormal, glm::normalize(windForce));
                glm::vec3 impulse = windForce * exposure * dt2 / 3.0f;

                if (!particles[a].pinned)  particles[a].position  += impulse;
                if (!particles[b].pinned)  particles[b].position  += impulse;
                if (!particles[c].pinned) particles[c].position += impulse;
            };

            applyTri(i00, i10, i01);
            applyTri(i10, i11, i01);
        }
    }
}

void ClothMesh::update(float dt) {
    if (!std::isfinite(dt) || dt <= 0.0f) return;

    // Guard against large frame-time spikes (e.g. window drag on Windows)
    constexpr float maxSimDt = 1.0f / 60.0f;
    constexpr float maxSubstepDt = 1.0f / 120.0f;

    float clampedDt = std::min(dt, maxSimDt);
    int substeps = std::max(1, static_cast<int>(std::ceil(clampedDt / maxSubstepDt)));
    float stepDt = clampedDt / static_cast<float>(substeps);

    for (int step = 0; step < substeps; ++step) {
        // Verlet integration
        for (auto& p : particles) {
            if (p.pinned) continue;
            glm::vec3 vel = (p.position - p.prevPosition) * damping;
            glm::vec3 next = p.position + vel + gravity * (stepDt * stepDt);
            p.prevPosition = p.position;
            p.position = next;
        }

        applyWind(stepDt);
        solveConstraints();
    }

    recalcNormals();
    uploadToGPU();
}

void ClothMesh::solveConstraints() {
    for (int iter = 0; iter < solverIterations; ++iter) {
        for (auto& s : springs) {
            auto& a = particles[s.a];
            auto& b = particles[s.b];

            glm::vec3 delta = b.position - a.position;
            float dist = glm::length(delta);
            if (dist < 1e-6f) continue;

            float correction = (dist - s.restLength) / dist;
            glm::vec3 offset = delta * correction * 0.5f;

            if (!a.pinned) a.position += offset;
            if (!b.pinned) b.position -= offset;
        }
    }
}

void ClothMesh::recalcNormals() {
    for (auto& p : particles) p.normal = glm::vec3(0.0f);

    // Accumulate face normals
    for (int row = 0; row < rows - 1; row++) {
        for (int col = 0; col < cols - 1; col++) {
            int i00 = row * cols + col;
            int i10 = i00 + 1;
            int i01 = i00 + cols;
            int i11 = i00 + cols + 1;

            auto triNormal = [&](int a, int b, int c) {
                glm::vec3 n = glm::cross(
                    particles[b].position - particles[a].position,
                    particles[c].position - particles[a].position
                );
                particles[a].normal += n;
                particles[b].normal += n;
                particles[c].normal += n;
            };

            triNormal(i00, i10, i01);
            triNormal(i10, i11, i01);
        }
    }

    for (auto& p : particles) {
        float len = glm::length(p.normal);
        if (len > 1e-6f) p.normal /= len;
    }
}

void ClothMesh::uploadToGPU() {
    for (int i = 0; i < (int)particles.size(); ++i) {
        cpuVertices[i] = { particles[i].position, particles[i].normal, particles[i].uv };
    }

    vertices = cpuVertices;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    cpuVertices.size() * sizeof(Vertex),
                    cpuVertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
