#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Texture/TextureManager.hpp"

#include <iostream>

MaterialManager& MaterialManager::instance() {
    static MaterialManager instance;
    return instance;
}

void MaterialManager::add(const std::string& name, std::shared_ptr<Material> material) {
    this->materials[name] = std::move(material);
}

void MaterialManager::addPBR(const std::string& name, const std::string& texturePrefix, float normalStrength) {
    auto& tex = TextureManager::instance();
    auto mat = std::make_shared<PBRMaterial>(
        tex.get(texturePrefix + "-albedo"),
        tex.get(texturePrefix + "-ao"),
        tex.get(texturePrefix + "-metallic"),
        tex.get(texturePrefix + "-normal"),
        tex.get(texturePrefix + "-roughness")
    );
    mat->normalStrength = normalStrength;
    this->materials[name] = std::move(mat);
}

std::shared_ptr<Material> MaterialManager::get(const std::string& name) {
    if (!this->materials.count(name)) {
        std::cerr << "[MaterialManager] material named " << name << " not found" << std::endl;
        exit(1);
    }

    return this->materials[name];
}