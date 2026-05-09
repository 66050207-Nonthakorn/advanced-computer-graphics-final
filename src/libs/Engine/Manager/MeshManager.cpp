#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Mesh/MeshLoader.hpp"
#include "Engine/Manager/MaterialManager.hpp"

#include <filesystem>
#include <iostream>

MeshManager& MeshManager::instance() {
    static MeshManager instance;
    return instance;
}

std::shared_ptr<Mesh> MeshManager::get(const std::string &name) {
    if (this->meshes.count(name)) {
        return meshes[name];
    }

    MeshData data;
    if (name == "cube") {
        data = MeshData::Factory::createCube();
    }
    else if (name == "triangle") {
        data = MeshData::Factory::createTriangle();
    }
    else if (name == "sphere") {
        data = MeshData::Factory::createSphere();
    }
    else if (name == "plane") {
        data = MeshData::Factory::createPlane();
    }
    else if (name == "quad") {
        data = MeshData::Factory::createQuad();
    }
    else {
        std::cerr << "[MeshManager] Mesh named " << name << " not found " << std::endl;
        exit(1);
    }

    return meshes[name] = std::make_shared<Mesh>(data);
}

std::shared_ptr<Mesh> MeshManager::getFromFile(const std::string &path) {
    if (this->meshes.count(path)) {
        return meshes[path];
    }

    auto data = MeshLoader::loadFromObj(path);
    return meshes[path] = std::make_shared<Mesh>(data);
}
