#include "Engine/Material/Material.hpp"

Material::Material(const std::shared_ptr<Shader> shader)
    : shader(shader) { }