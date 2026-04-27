#include "Engine/Texture/TextureManager.hpp"

#include "Engine/Texture/Texture2D.hpp"

#include "gl/glew.h"
#include "stb_image.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

TextureManager& TextureManager::instance() {
    static TextureManager instance;
    return instance;
}

void TextureManager::add(const std::string& name, const std::shared_ptr<Texture>& texture) {
    if (this->textures.count(name)) {
        std::cout << "[TextureManager] " << name << " already exist";
        return;
    }

    this->textures[name] = texture;
}

bool TextureManager::has(const std::string& name) const {
    return this->textures.count(name) > 0;
}

void TextureManager::addPBR(const std::string& name, const std::string& dir) {
    struct PBRSlot {
        std::string keyword;
        bool isSRGB;
    };
    static const PBRSlot slots[] = {
        { "albedo",    true  },
        { "ao",        false },
        { "metallic",  false },
        { "normal",    false },
        { "roughness", false }
    };

    namespace fs = std::filesystem;
    if (!fs::is_directory(dir)) {
        std::cerr << "[TextureManager] " << dir << " is not a directory or not exists" << std::endl;
        return;
    }

    for (const auto& slot : slots) {
        std::string found = "";

        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
        
            std::string lower = entry.path().filename().string();
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            if (lower.find(slot.keyword) != std::string::npos) {
                found = entry.path().string();
                break;
            }
        }

        if (found.empty()) {
            std::cerr << "[TextureManager] no " << slot.keyword << " texture found in " << dir << std::endl;
            continue;
        }
        
        add(name + "-" + slot.keyword, std::make_shared<Texture2D>(found, slot.isSRGB));
    }

    std::cout << "[TextureManager] loaded " << dir << std::endl;
}

std::shared_ptr<Texture> TextureManager::get(const std::string &name) {
    if (!this->textures.count(name)) {
        std::cerr << "[TextureManager] texture named " << name << " is not exits" << std::endl;
        exit(1);
    }

    return this->textures[name];
}