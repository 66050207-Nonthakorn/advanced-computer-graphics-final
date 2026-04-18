#pragma once

#include "MeshData.hpp"
#include <string>

class MeshLoader {
public:
    static MeshData loadFromObj(const std::string& path);
};