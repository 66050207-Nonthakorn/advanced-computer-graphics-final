#include "Main/MainScene.hpp"

#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Light/PointLight.hpp"

#include "glfw/glfw3.h"

#include <iostream>

MainScene::MainScene() {
    SceneObject object;
    object.mesh = MeshManager::instance().get("sphere");
    object.material = MaterialManager::instance().get("cloth");

    SceneObject object2;
    object2.transform.setPosition({2, 0, 0});
    object2.mesh = MeshManager::instance().get("sphere");
    object2.material = MaterialManager::instance().get("brass");

    SceneObject plane;
    plane.mesh = MeshManager::instance().get("plane");
    plane.material = MaterialManager::instance().get("boulders");
    plane.transform.setPosition({0, -2, 0});
    plane.transform.setScale({10, 1, 10});
    // plane.

    PointLight light;
    light.position = {0.0, 2.5, 0.0};
    light.color = {1.0, 1.0, 1.0};
    light.intensity = 7.0f;

    this->directionalLight.direction = glm::normalize(glm::vec3(1.0f, -1.0f, -0.5f));
    this->directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    this->directionalLight.intensity = 10.0f;

    this->sceneObjects.emplace_back(object);
    this->sceneObjects.emplace_back(object2);
    this->sceneObjects.emplace_back(plane);

    this->lights.emplace_back(light);
}

void MainScene::update(float dt) {
    // auto rotate = sceneObjects[0].transform.getRotation();
    sceneObjects[0].transform.setRotation({glfwGetTime() * 20.0f, glfwGetTime() * 20.0f, glfwGetTime() * 20.0f});
}