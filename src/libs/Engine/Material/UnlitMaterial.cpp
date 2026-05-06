#include "Engine/Material/UnlitMaterial.hpp"
#include "Engine/Manager/ShaderManager.hpp"

UnlitMaterial::UnlitMaterial(const glm::vec4& color)
    : Material(ShaderManager::instance().get("unlit")), color(color) {}

UnlitMaterial::UnlitMaterial(const std::shared_ptr<Texture>& texture, const glm::vec4& color)
    : UnlitMaterial(color)
{
    this->texture = texture;
}

void UnlitMaterial::bindPerFrame(const Material::PerFrameContext& context) {
    shader->use();
    shader->uniformMat4("view", context.camera.getView());
    shader->uniformMat4("projection", context.camera.getProjection());

    const bool hasTexture = static_cast<bool>(texture);
    shader->uniformBool("useTexture", hasTexture);
    shader->uniformVec4("color", color);
    if (hasTexture) {
        shader->bindTexture(0, texture->getId());
        shader->uniformInt("diffuse", 0);
    }
}

void UnlitMaterial::bindPerObject(const Material::PerObjectContext& context) {
    shader->uniformMat4("model", context.modelMatrix);
}
