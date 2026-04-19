#include "Engine/Material/BlinnPhongMaterial.hpp"
#include "Engine/Shader/ShaderManager.hpp"
#include "Engine/Transform.hpp"

BlinnPhongMaterial::BlinnPhongMaterial()
    : Material(ShaderManager::instance().get("blinn-phong")) { }

BlinnPhongMaterial::BlinnPhongMaterial(const std::shared_ptr<Texture> &texture)
    : Material(ShaderManager::instance().get("blinn-phong")),
      texture(texture) {}

void BlinnPhongMaterial::bindPerFrame(const MaterialPerFrameContext& context) {
    this->shader->use();
    this->shader->uniformMat4("view", context.camera.getView());
    this->shader->uniformMat4("projection", context.camera.getProjection());
    this->shader->uniformVec3("viewPosition", context.camera.position);
    this->shader->uniformVec3("lightPosition", context.lights[0].position);
}

void BlinnPhongMaterial::bindPerObject(const MaterialPerObjectContext& context) {
    this->shader->uniformMat4("model", context.transform.getModel());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture->id);
    this->shader->uniformInt("objectTexture", 0);
}