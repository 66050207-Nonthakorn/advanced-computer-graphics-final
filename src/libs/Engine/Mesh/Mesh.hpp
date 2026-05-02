#pragma once

#include "MeshData.hpp"
#include <vector>

struct Mesh {
public:
    unsigned int vao, vbo, ebo;
    size_t indexCount = 0;
    
    Mesh(const MeshData&);
    virtual ~Mesh();
};