#include "ParticleMaterial.hpp"
#include "Engine/Manager/ShaderManager.hpp"

ParticleMaterial::ParticleMaterial()
    : Material(ShaderManager::instance().get("particle")) {}

void ParticleMaterial::bindPerFrame(const Material::PerFrameContext& context) {
    shader->use();
    shader->uniformMat4("view",       context.camera.getView());
    shader->uniformMat4("projection", context.camera.getProjection());
}

void ParticleMaterial::bindPerObject(const Material::PerObjectContext& context) {
    shader->uniformMat4("model", context.modelMatrix);
}