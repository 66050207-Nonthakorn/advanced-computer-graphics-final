#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Shader/ShaderManager.hpp"

#include "GL/glew.h"
#include "Engine/Light/DirectionalLight.hpp"
#include <algorithm>
#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void Renderer::draw(Scene& scene) {
    // Shadow pass
    glm::mat4 lightSpaceMat = scene.directionalLight.getLightSpaceMatrix();
    auto shadowShader = ShaderManager::instance().get("shadow");

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, scene.directionalLight.getShadowFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    shadowShader->use();
    shadowShader->uniformMat4("lightSpaceMatrix", lightSpaceMat);

    for (auto& sceneObject : scene.sceneObjects) {
        shadowShader->uniformMat4("model", sceneObject.transform.getModel());
        
        glBindVertexArray(sceneObject.mesh->vao);
        glDrawElements(GL_TRIANGLES, sceneObject.mesh->indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Lighting pass
    std::vector<SceneObject> sceneObjects(scene.sceneObjects.begin(), scene.sceneObjects.end());
    std::sort(sceneObjects.begin(), sceneObjects.end(),
        [](const auto& a, const auto& b) {
            return a.material.get() < b.material.get();
        });

    Material* lastMaterial = nullptr;
    for (auto& sceneObject : sceneObjects) {
        Material* mat = sceneObject.material.get();
        if (mat != lastMaterial) {
            mat->bindPerFrame({ scene.camera, scene.lights, scene.directionalLight,
                                scene.directionalLight.getDepthMapID(), lightSpaceMat });
            lastMaterial = mat;
        }
        
        sceneObject.material->bindPerObject({ sceneObject.transform });
        
        glBindVertexArray(sceneObject.mesh->vao);
        glDrawElements(GL_TRIANGLES, sceneObject.mesh->indexCount, GL_UNSIGNED_INT, 0);
    }

    // Cubemap
    if (scene.cubemap.get() != nullptr) {
        auto cubemapShader = ShaderManager::instance().get("cubemap");
        cubemapShader->use();
        cubemapShader->uniformMat4("view", glm::mat4(glm::mat3(scene.camera.getView())));
        cubemapShader->uniformMat4("projection", scene.camera.getProjection());
        cubemapShader->uniformInt("skybox", 0);

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        scene.cubemap->texture->bind(0);
        glBindVertexArray(scene.cubemap->cube->vao);
        glDrawElements(GL_TRIANGLES, scene.cubemap->cube->indexCount, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }
}
