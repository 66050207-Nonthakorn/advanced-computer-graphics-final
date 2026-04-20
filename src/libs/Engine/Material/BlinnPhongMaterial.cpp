#include "Engine/Material/BlinnPhongMaterial.hpp"
#include "Engine/Shader/ShaderManager.hpp"
#include "Engine/Transform.hpp"

BlinnPhongMaterial::BlinnPhongMaterial()
    : Material(ShaderManager::instance().get("blinn-phong")),
      ambient(0.1f), diffuse(0.8f), specular(0.2f), shininess(32.0f) { }

BlinnPhongMaterial::BlinnPhongMaterial(const std::shared_ptr<Texture> &texture)
    : BlinnPhongMaterial::BlinnPhongMaterial()
{
    this->texture = texture;
}

void BlinnPhongMaterial::bindPerFrame(const Material::PerFrameContext& context) {
    this->shader->use();

    this->shader->uniformMat4("view", context.camera.getView());
    this->shader->uniformVec3("viewPosition", context.camera.position);
    this->shader->uniformMat4("projection", context.camera.getProjection());
    
    this->shader->uniformVec3("light.position", context.lights[0].position);
    this->shader->uniformFloat("light.ambient", context.lights[0].ambient);
    this->shader->uniformFloat("light.diffuse", context.lights[0].diffuse);
    this->shader->uniformFloat("light.specular", context.lights[0].specular);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture->id);

    this->shader->uniformInt("material.diffuse", 0);
    this->shader->uniformFloat("material.specular", this->specular);
    this->shader->uniformFloat("material.shininess", this->shininess);
}

void BlinnPhongMaterial::bindPerObject(const Material::PerObjectContext& context) {
    this->shader->uniformMat4("model", context.transform.getModel());
}