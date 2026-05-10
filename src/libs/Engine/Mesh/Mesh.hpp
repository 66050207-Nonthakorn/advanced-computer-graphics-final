#pragma once

#include "MeshData.hpp"
#include "glm/vec3.hpp"
#include <vector>

struct Mesh {
public:
    unsigned int vao, vbo, ebo;
    unsigned int drawMode = 0;
    size_t indexCount = 0;
    std::vector<Vertex> vertices;

    Mesh(const MeshData&, unsigned int drawMode = 0x0004);
    virtual ~Mesh() noexcept;
};