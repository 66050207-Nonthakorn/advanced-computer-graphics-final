#include "Engine/Material/MaterialManager.hpp"

#include <iostream>

MaterialManager& MaterialManager::instance() {
    static MaterialManager instance;
    return instance;
}

void MaterialManager::add(const std::string& name, std::shared_ptr<Material> material) {
    this->materials[name] = std::move(material);
}

std::shared_ptr<Material> MaterialManager::get(const std::string& name) {
    if (!this->materials.count(name)) {
        std::cerr << "[MaterialManager] material named " << name << " not found" << std::endl;
        exit(1);
    }

    return this->materials[name];
}