#include "Engine/Texture/TextureManager.hpp"

#include "gl/glew.h"
#include "stb_image.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

TextureManager& TextureManager::instance() {
    static TextureManager instance;
    return instance;
}

void TextureManager::load(const std::string& name, const std::string& path, const TextureType& type, const bool isFlip) {
    if (this->textures.count(name)) {
        std::cout << "[TextureManager] " << name << " already exist";
        return;
    }

    int textureFormat = GL_RGB;
    switch (type) {
        case TextureType::NORMAL_MAP: textureFormat = GL_RGB;  break;
        case TextureType::DEFAULT:    textureFormat = GL_SRGB; break;
    }

    this->textures[name] = std::make_shared<Texture>(path, textureFormat, isFlip);
}

void TextureManager::loadAsync(const std::string& name, const std::string& path, const TextureType& type, const bool isFlip) {
    if (this->textures.count(name)) {
        std::cout << "[TextureManager] " << name << " already exist";
        return;
    }

    int textureFormat = GL_RGB;
    switch (type) {
        case TextureType::NORMAL_MAP: textureFormat = GL_RGB;  break;
        case TextureType::DEFAULT:    textureFormat = GL_SRGB; break;
    }

    this->threads.emplace_back([this, name, path, textureFormat, isFlip]() {
        // stbi_set_flip_vertically_on_load_thread is thread-local — safe to call per-thread
        stbi_set_flip_vertically_on_load_thread(isFlip);

        int w, h, c;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
        if (!data) {
            std::cerr << "[TextureManager] failed to load " << path << std::endl;
            return;
        }

        std::lock_guard<std::mutex> lock(this->pendingMutex);
        this->pending.push_back({ name, data, w, h, c, textureFormat });
    });
}

void TextureManager::loadPBRAsync(const std::string& name, const std::string& dir) {
    struct PBRSlot {
        std::string suffix;
        std::string keyword;
        TextureType type;
    };
    static const PBRSlot slots[] = {
        { "-albedo",    "albedo",    TextureType::DEFAULT    },
        { "-ao",        "ao",        TextureType::DEFAULT    },
        { "-metallic",  "metallic",  TextureType::DEFAULT    },
        { "-normal",    "normal",    TextureType::NORMAL_MAP },
        { "-roughness", "roughness", TextureType::DEFAULT    },
    };

    namespace fs = std::filesystem;
    for (const auto& slot : slots) {
        std::string found;
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
        
        loadAsync(name + slot.suffix, found, slot.type);
    }
}

void TextureManager::finalize() {
    for (auto& t : this->threads) t.join();
    this->threads.clear();

    for (auto& p : this->pending) {
        this->textures[p.name] = std::make_shared<Texture>(p.data, p.width, p.height, p.nrChannels, p.format);
        stbi_image_free(p.data);
    }
    this->pending.clear();
}

std::shared_ptr<Texture> TextureManager::get(const std::string &name) {
    if (!this->textures.count(name)) {
        std::cerr << "[TextureManager] texture named " << name << " is not exits" << std::endl;
        exit(1);
    }

    return this->textures[name];
}