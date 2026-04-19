#include "Main/MainScene.hpp"

#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Light.hpp"

#include "glfw/glfw3.h"

#include <iostream>

MainScene::MainScene() {
    SceneObject object;
    object.mesh = MeshManager::instance().get("cube");
    object.material = MaterialManager::instance().get("dirt");

    SceneObject lightCube;
    object.mesh = MeshManager::instance().get("cube");
    object.material = MaterialManager::instance().get("dirt");

    Light light;
    light.position = glm::vec3(0, 0, 0);

    this->sceneObjects.emplace_back(object);
    this->lights.emplace_back(light);
}

void MainScene::update(float dt) {
    lights[0].position.x = sin(glfwGetTime()) * 2;
    lights[0].position.z = cos(glfwGetTime()) * 2;
}