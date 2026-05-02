#include "Engine/Manager/ShaderManager.hpp"

#include <iostream>

ShaderManager& ShaderManager::instance() {
    static ShaderManager instance;
    return instance;
}

void ShaderManager::load(
    const std::string &name,
    const std::string &vertexPath,
    const std::string &fragmentPath
) {
    if (this->shaders.count(name)) {
        std::cerr << "[ShaderManager] shader named " << name << " already exists" << std::endl;
        exit(1);
    }

    auto shader = std::make_shared<Shader>(vertexPath, fragmentPath);
    this->shaders[name] = shader;
}

std::shared_ptr<Shader> ShaderManager::get(const std::string &name) {
    if (!this->shaders.count(name)) {
        std::cerr << "[ShaderManager] shader named " << name << " is not found" << std::endl;
        exit(1);
    }

    return shaders[name];
}
