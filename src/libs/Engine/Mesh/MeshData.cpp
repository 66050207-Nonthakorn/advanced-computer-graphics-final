#include "Engine/Mesh/MeshData.hpp"
#include <cmath>

static constexpr float PI = 3.14159265358979323846f;

MeshData MeshData::Factory::createCube() {
    MeshData meshData;
    meshData.vertices = {
        // Front  (z = +1)
        {{-1, -1,  1}, { 0,  0,  1}, {0, 0}},
        {{ 1, -1,  1}, { 0,  0,  1}, {1, 0}},
        {{ 1,  1,  1}, { 0,  0,  1}, {1, 1}},
        {{-1,  1,  1}, { 0,  0,  1}, {0, 1}},
        // Back   (z = -1)
        {{ 1, -1, -1}, { 0,  0, -1}, {0, 0}},
        {{-1, -1, -1}, { 0,  0, -1}, {1, 0}},
        {{-1,  1, -1}, { 0,  0, -1}, {1, 1}},
        {{ 1,  1, -1}, { 0,  0, -1}, {0, 1}},
        // Left   (x = -1)
        {{-1, -1, -1}, {-1,  0,  0}, {0, 0}},
        {{-1, -1,  1}, {-1,  0,  0}, {1, 0}},
        {{-1,  1,  1}, {-1,  0,  0}, {1, 1}},
        {{-1,  1, -1}, {-1,  0,  0}, {0, 1}},
        // Right  (x = +1)
        {{ 1, -1,  1}, { 1,  0,  0}, {0, 0}},
        {{ 1, -1, -1}, { 1,  0,  0}, {1, 0}},
        {{ 1,  1, -1}, { 1,  0,  0}, {1, 1}},
        {{ 1,  1,  1}, { 1,  0,  0}, {0, 1}},
        // Top    (y = +1)
        {{-1,  1,  1}, { 0,  1,  0}, {0, 0}},
        {{ 1,  1,  1}, { 0,  1,  0}, {1, 0}},
        {{ 1,  1, -1}, { 0,  1,  0}, {1, 1}},
        {{-1,  1, -1}, { 0,  1,  0}, {0, 1}},
        // Bottom (y = -1)
        {{-1, -1, -1}, { 0, -1,  0}, {0, 0}},
        {{ 1, -1, -1}, { 0, -1,  0}, {1, 0}},
        {{ 1, -1,  1}, { 0, -1,  0}, {1, 1}},
        {{-1, -1,  1}, { 0, -1,  0}, {0, 1}},
    };

    meshData.indices = {
         0,  1,  2,  2,  3,  0,  // Front
         4,  5,  6,  6,  7,  4,  // Back
         8,  9, 10, 10, 11,  8,  // Left
        12, 13, 14, 14, 15, 12,  // Right
        16, 17, 18, 18, 19, 16,  // Top
        20, 21, 22, 22, 23, 20,  // Bottom
    };
    
    return meshData;
}

MeshData MeshData::Factory::createTriangle() {
    MeshData meshData;
    meshData.vertices = {
        {{ 0,  1, 0}, {1, 0, 0}, {0.5, 1}},
        {{ 1, -1, 0}, {0, 1, 0}, {  1, 0}},
        {{-1, -1, 0}, {0, 0, 1}, {  0, 0}},
    };
    meshData.indices = { 0, 1, 2 };
    return meshData;
}

MeshData MeshData::Factory::createSphere(int stacks, int slices) {
    MeshData meshData;

    for (int i = 0; i <= stacks; ++i) {
        float phi = PI * i / stacks; // 0 .. pi
        float y   = std::cos(phi);
        float r   = std::sin(phi);
        float v   = static_cast<float>(i) / stacks;

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * PI * j / slices; // 0 .. 2pi
            float x = r * std::cos(theta);
            float z = r * std::sin(theta);
            float u = static_cast<float>(j) / slices;

            meshData.vertices.push_back({{x, y, z}, {x, y, z}, {u, v}});
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned int a = i * (slices + 1) + j;
            unsigned int b = a + (slices + 1);

            meshData.indices.insert(meshData.indices.end(), {a, b, a + 1, b, b + 1, a + 1});
        }
    }

    return meshData;
}