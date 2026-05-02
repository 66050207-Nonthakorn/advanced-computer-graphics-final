#include "Scene.hpp"

void Scene::update(float dt) {
    for (auto& sceneObject: this->sceneObjects) {
        sceneObject->update(dt);
    }
}