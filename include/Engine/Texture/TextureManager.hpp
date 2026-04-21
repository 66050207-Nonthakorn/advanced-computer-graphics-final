#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "Texture.hpp"
#include "TextureType.hpp"

class TextureManager {
public:
    static TextureManager& instance();

    TextureManager(TextureManager &other) = delete;
    TextureManager& operator=(const TextureManager &) = delete;

    void load(
        const std::string& name,
        const std::string& path,
        const TextureType& type = TextureType::DEFAULT,
        const bool isFlip = true
    );

    void loadAsync(
        const std::string& name,
        const std::string& path,
        const TextureType& type = TextureType::DEFAULT,
        const bool isFlip = true
    );

    void loadPBRAsync(const std::string& name, const std::string& dir);
    void finalize();

    std::shared_ptr<Texture> get(const std::string& name);

private:
    struct PendingTexture {
        std::string name;
        unsigned char* data;
        int width, height, nrChannels;
        int format;
    };

    TextureManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

    std::vector<std::thread> threads;
    std::vector<PendingTexture> pending;
    std::mutex pendingMutex;
};