#include "Engine/Manager/MaterialManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Manager/TextureManager.hpp"
#include "Engine/Texture/Texture2D.hpp"

#include "glm/glm.hpp"
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
    auto getOptional = [&tex](const std::string& textureName) -> std::shared_ptr<Texture> {
        return tex.has(textureName) ? tex.get(textureName) : nullptr;
    };

    auto mat = std::make_shared<PBRMaterial>(
        getOptional(texturePrefix + "-albedo"),
        getOptional(texturePrefix + "-ao"),
        getOptional(texturePrefix + "-metallic"),
        getOptional(texturePrefix + "-normal"),
        getOptional(texturePrefix + "-roughness")
    );
    mat->normalStrength = normalStrength;
    this->materials[name] = std::move(mat);
}

bool MaterialManager::has(const std::string& name) const {
    return this->materials.count(name) > 0;
}

std::shared_ptr<Material> MaterialManager::get(const std::string& name) {
    if (!this->materials.count(name)) {
        std::cerr << "[MaterialManager] material named " << name << " not found" << std::endl;
        exit(1);
    }

    return this->materials[name];
}
