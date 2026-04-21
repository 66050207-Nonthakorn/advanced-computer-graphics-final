#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"

#include "GL/glew.h"
#include <algorithm>

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void Renderer::draw(
    Scene& scene,
    const Camera& camera
) {
    // Draw Object
    // Copy vector to keep original order 
    std::vector<SceneObject> sceneObjects(scene.sceneObjects.begin(), scene.sceneObjects.end());
    std::sort(sceneObjects.begin(), sceneObjects.end(),
        [](const auto& a, const auto& b) {
            return a.material.get() < b.material.get();
        });

    Material* lastMaterial = nullptr;
    for (auto& sceneObject : sceneObjects) {
        // Bind view once for the same material
        Material* mat = sceneObject.material.get();
        if (mat != lastMaterial) {
            mat->bindPerFrame({ camera, scene.lights, scene.directionalLight });
            lastMaterial = mat;
        }
        
        // Bind for every objects
        sceneObject.material->bindPerObject({ sceneObject.transform });
        
        // Draw
        glBindVertexArray(sceneObject.mesh->vao);
        glDrawElements(GL_TRIANGLES, sceneObject.mesh->indexCount, GL_UNSIGNED_INT, 0);
    }
}
