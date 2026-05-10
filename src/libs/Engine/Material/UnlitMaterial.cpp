#include "Engine/Material/UnlitMaterial.hpp"
#include "Engine/Manager/ShaderManager.hpp"

UnlitMaterial::UnlitMaterial(const glm::vec4& color)
    : Material(ShaderManager::instance().get("unlit")), color(color) {
}

void UnlitMaterial::bindPerFrame(const Material::PerFrameContext& context) {
    shader->use();
    shader->uniformMat4("view", context.camera.getView());
    shader->uniformMat4("projection", context.camera.getProjection());
    shader->uniformVec4("color", color);
}

void UnlitMaterial::bindPerObject(const Material::PerObjectContext& context) {
    shader->uniformMat4("model", context.modelMatrix);
}
