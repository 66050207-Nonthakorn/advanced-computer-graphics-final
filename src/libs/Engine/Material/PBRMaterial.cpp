#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Shader/ShaderManager.hpp"

PBRMaterial::PBRMaterial(
    const std::shared_ptr<Texture>& albedo,
    const std::shared_ptr<Texture>& ao,
    const std::shared_ptr<Texture>& metallic,
    const std::shared_ptr<Texture>& normal,
    const std::shared_ptr<Texture>& roughness
) : Material(ShaderManager::instance().get("pbr")),
    albedo(albedo), ao(ao), metallic(metallic), normal(normal), roughness(roughness) { }

void PBRMaterial::bindPerFrame(const Material::PerFrameContext& context) {
    this->shader->use();

    const bool hasAlbedo = static_cast<bool>(this->albedo);
    const bool hasAo = static_cast<bool>(this->ao);
    const bool hasMetallic = static_cast<bool>(this->metallic);
    const bool hasNormal = static_cast<bool>(this->normal);
    const bool hasRoughness = static_cast<bool>(this->roughness);

    this->shader->bindTexture(0, hasAlbedo ? this->albedo->getId() : 0);
    this->shader->bindTexture(1, hasAo ? this->ao->getId() : 0);
    this->shader->bindTexture(2, hasMetallic ? this->metallic->getId() : 0);
    this->shader->bindTexture(3, hasNormal ? this->normal->getId() : 0);
    this->shader->bindTexture(4, hasRoughness ? this->roughness->getId() : 0);

    this->shader->uniformInt("material.albedo", 0);
    this->shader->uniformInt("material.ao", 1);
    this->shader->uniformInt("material.metallic", 2);
    this->shader->uniformInt("material.normal", 3);
    this->shader->uniformInt("material.roughness", 4);
    this->shader->uniformBool("useAlbedoMap", hasAlbedo);
    this->shader->uniformBool("useAoMap", hasAo);
    this->shader->uniformBool("useMetallicMap", hasMetallic);
    this->shader->uniformBool("useNormalMap", hasNormal);
    this->shader->uniformBool("useRoughnessMap", hasRoughness);

    this->shader->uniformMat4("view", context.camera.getView());
    this->shader->uniformMat4("projection", context.camera.getProjection());
    this->shader->uniformVec3("viewPosition", context.camera.position);
    
    this->shader->uniformInt("pointLightCount", context.lights.size());

    for (size_t i = 0; i < context.lights.size(); i++) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        this->shader->uniformVec3(base + ".position", context.lights[i].position);
        this->shader->uniformVec3(base + ".color", context.lights[i].color);
        this->shader->uniformFloat(base + ".intensity", context.lights[i].intensity);
    }

    this->shader->uniformVec3("directionalLight.direction", context.directionalLight.direction);
    this->shader->uniformVec3("directionalLight.color", context.directionalLight.color);
    this->shader->uniformFloat("directionalLight.intensity", context.directionalLight.intensity);

    this->shader->bindTexture(5, context.dirShadowMap);
    this->shader->uniformInt("shadowMap", 5);
    this->shader->uniformMat4("lightSpaceMatrix", context.lightSpaceMatrix);

    this->shader->uniformFloat("normalStrength", this->normalStrength);
}

void PBRMaterial::bindPerObject(const Material::PerObjectContext& context) {
    this->shader->uniformMat4("model", context.transform.getModel());
}