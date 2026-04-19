#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Texture.hpp"

class TextureManager {
public:
    static TextureManager& instance();

    TextureManager(TextureManager &other) = delete;
    TextureManager& operator=(const TextureManager &) = delete;

    void load(const std::string& name, const std::string& path);
    std::shared_ptr<Texture> get(const std::string& name);

private:
    TextureManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};