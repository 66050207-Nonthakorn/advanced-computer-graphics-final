#include "Engine/Material/BillboardMaterial.hpp"
#include "Engine/Manager/ShaderManager.hpp"

BillboardMaterial::BillboardMaterial(const glm::vec4& color)
    : Material(ShaderManager::instance().get("billboard")), color(color) {}

BillboardMaterial::BillboardMaterial(const std::shared_ptr<Texture>& texture, const glm::vec4& color)
    : BillboardMaterial(color)
{
    this->texture = texture;
}

void BillboardMaterial::bindPerFrame(const Material::PerFrameContext& context) {
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

void BillboardMaterial::bindPerObject(const Material::PerObjectContext& context) {
    shader->uniformMat4("model", context.modelMatrix);
}
