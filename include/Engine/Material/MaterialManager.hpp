#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "Material.hpp"

class MaterialManager {
public:
    static MaterialManager& instance();

    MaterialManager(MaterialManager &other) = delete;
    MaterialManager& operator=(const MaterialManager &) = delete;

    void add(const std::string& name, std::shared_ptr<Material> material);
    std::shared_ptr<Material> get(const std::string& name);

private:
    MaterialManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
};