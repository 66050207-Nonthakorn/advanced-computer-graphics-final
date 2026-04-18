#pragma once

#include <unordered_map>
#include <memory>

#include "Shader.hpp"

class ShaderManager {
public:
    static ShaderManager& instance();

    ShaderManager(ShaderManager&) = delete;
    ShaderManager& operator=(ShaderManager&) = delete;

    void load(
        const std::string& name,
        const std::string& vertexPath,
        const std::string& fragmentPath
    );

    std::shared_ptr<Shader> get(const std::string& name);

private:
    ShaderManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders; 
};