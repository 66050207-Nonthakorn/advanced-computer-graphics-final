#include "Engine/Renderer.hpp"

#include "GL/glew.h"

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Renderer::draw(std::vector<std::shared_ptr<SceneObject>>& sceneObjects, const Camera& camera) {
    for (auto& sceneObject: sceneObjects) {
        sceneObject->draw(camera);
    }
}
