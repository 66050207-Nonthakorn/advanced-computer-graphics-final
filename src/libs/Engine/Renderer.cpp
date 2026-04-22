#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Shader/ShaderManager.hpp"

#include "GL/glew.h"
#include "Engine/Light/DirectionalLight.hpp"
#include <algorithm>

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void Renderer::draw(Scene& scene) {
    // --- Shadow pass ---
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

    // --- Lighting pass ---
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
}
