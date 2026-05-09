#include "Scene.hpp"

void Scene::update(float dt) {
    for (auto& sceneObject: this->sceneObjects) {
        this->updateSceneObject(sceneObject, dt);
    }
}

void Scene::updateSceneObject(const std::shared_ptr<SceneObject>& obj, float dt) {
    obj->update(dt);
    for (auto& child : obj->children) {
        this->updateSceneObject(child, dt);
    }
}