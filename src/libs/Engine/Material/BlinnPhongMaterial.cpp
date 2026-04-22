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
    
    this->shader->uniformInt("pointLightCount", (int)context.lights.size());
    for (size_t i = 0; i < context.lights.size(); i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        this->shader->uniformVec3(base + ".position", context.lights[i].position);
        this->shader->uniformVec3(base + ".color",    context.lights[i].color);
        this->shader->uniformFloat(base + ".intensity", context.lights[i].intensity);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture->id);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, context.dirShadowMap);

    this->shader->uniformInt("material.diffuse", 0);
    this->shader->uniformFloat("material.specular", this->specular);
    this->shader->uniformFloat("material.shininess", this->shininess);
    this->shader->uniformInt("shadowMap", 1);
    this->shader->uniformMat4("lightSpaceMatrix", context.lightSpaceMatrix);

    this->shader->uniformVec3("directionalLight.direction", context.directionalLight.direction);
    this->shader->uniformVec3("directionalLight.color", context.directionalLight.color);
    this->shader->uniformFloat("directionalLight.intensity", context.directionalLight.intensity);
}

void BlinnPhongMaterial::bindPerObject(const Material::PerObjectContext& context) {
    this->shader->uniformMat4("model", context.transform.getModel());
}