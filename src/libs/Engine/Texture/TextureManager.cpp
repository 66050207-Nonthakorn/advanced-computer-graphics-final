#include "Engine/Texture/TextureManager.hpp"

#include <iostream>

TextureManager& TextureManager::instance() {
    static TextureManager instance;
    return instance;
}

void TextureManager::load(const std::string& name, const std::string& path) {
    if (this->textures.count(name)) {
        std::cout << "[TextureManager] " << name << " already exist";
        return;
    }

    this->textures[name] = std::make_shared<Texture>(path);
}

std::shared_ptr<Texture> TextureManager::get(const std::string &name) {
    if (!this->textures.count(name)) {
        std::cerr << "[TextureManager] texture named " << name << " is not exits" << std::endl;
        exit(1);
    }

    return this->textures[name];
}