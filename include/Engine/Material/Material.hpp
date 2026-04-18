#pragma once

#include "Engine/Shader/Shader.hpp"
#include <memory>

struct Material {
    Material(const std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> shader;
};