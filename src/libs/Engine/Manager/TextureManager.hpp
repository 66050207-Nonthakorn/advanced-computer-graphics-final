#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "Engine/Texture/Texture.hpp"

class TextureManager {
public:
    static TextureManager& instance();

    TextureManager(TextureManager &other) = delete;
    TextureManager& operator=(const TextureManager &) = delete;

    void add(const std::string& name, const std::shared_ptr<Texture>& texture);
    void addPBR(const std::string& name, const std::string& dir, bool isFlip = true);
    bool has(const std::string& name) const;

    std::shared_ptr<Texture> get(const std::string& name);

private:
    TextureManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};
