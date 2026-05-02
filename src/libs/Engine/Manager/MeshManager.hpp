#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Engine/Mesh/Mesh.hpp"

class MeshManager {
public:
    static MeshManager& instance();

    MeshManager(MeshManager &other) = delete;
    MeshManager& operator=(const MeshManager &) = delete;

    std::shared_ptr<Mesh> get(const std::string& name);
    std::shared_ptr<Mesh> getFromFile(const std::string& path);

private:
    MeshManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
};
