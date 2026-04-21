#pragma once

#include "Engine/Vertex.hpp"
#include <vector>

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    struct Factory {
        static MeshData createTriangle();

        static MeshData createCube();
        static MeshData createPlane();
        static MeshData createSphere(int stacks = 32, int slices = 32);
    };
};