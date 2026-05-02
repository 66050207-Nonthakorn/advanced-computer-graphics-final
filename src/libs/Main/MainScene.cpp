#include "Main/MainScene.hpp"
#include "Main/Constanst.hpp"

#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Manager/InputManager.hpp"
#include "Engine/Texture/TextureManager.hpp"

#include "glfw/glfw3.h"

#include <filesystem>
#include <iostream>

namespace {

float yaw = -90, pitch = 0, sensitivity = 0.2f;
float speed = 10.0f;

}

MainScene::MainScene() {
    float aspect = static_cast<float>(Constanst::SCREEN_WIDTH) / Constanst::SCREEN_HEIGHT;
    camera = Camera(60.0f, aspect, 0.1f, 100.0f);

    cubemap = HDRCubemap(
        TextureManager::instance().get("night-sky-hdr"),
        TextureManager::instance().get("brdf-lut-512")
    );

    auto object = std::make_shared<SceneObject>();
    object->mesh = MeshManager::instance().get("sphere");
    object->material = MaterialManager::instance().get("cloth");

    auto object2 = std::make_shared<SceneObject>();
    object2->transform.setPosition({2, 1, 0});
    object2->mesh = MeshManager::instance().get("sphere");
    object2->material = MaterialManager::instance().get("titanium");

    auto object3 = std::make_shared<SceneObject>();
    object3->transform.setPosition({4, 1, 0});
    object3->transform.setScale({10, 10, 10});
    object3->mesh = MeshManager::instance().getFromFile("Models/Bread.obj");
    object3->material = MaterialManager::instance().get("bread");

    auto face = std::make_shared<SceneObject>();
    face->transform.setPosition({2, 2, 0});
    face->transform.setScale({.01, .01, .01});
    face->mesh = MeshManager::instance().getFromFile("Models/Caligula.obj");
    face->material = MaterialManager::instance().get("titanium");

    auto face2 = std::make_shared<SceneObject>();
    face2->transform.setPosition({4, 2, 0});
    face2->transform.setScale({.01, .01, .01});
    face2->mesh = MeshManager::instance().getFromFile("Models/Caligula.obj");
    face2->material = MaterialManager::instance().get("gold");

    auto plane = std::make_shared<SceneObject>();
    plane->mesh = MeshManager::instance().get("plane");
    plane->material = MaterialManager::instance().get("metal");
    plane->transform.setPosition({0, -2, 0});
    plane->transform.setScale({10, 1, 10});

    PointLight light;
    light.position = {0.0, 2.5, 0.0};
    light.color = {1.0, 1.0, 1.0};
    light.intensity = 10.0f;

    this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    this->directionalLight.color = glm::vec3(0.2f, 0.2f, 0.2f);
    this->directionalLight.intensity = 1.0f;

    this->sceneObjects.push_back(object);
    this->sceneObjects.push_back(object2);
    this->sceneObjects.push_back(object3);
    this->sceneObjects.push_back(plane);
    this->sceneObjects.push_back(face);
    this->sceneObjects.push_back(face2);

    this->lights.emplace_back(light);
}

void MainScene::update(float dt) {
    glm::vec2 mouseDelta = InputManager::instance().getMouseDelta();
    yaw += mouseDelta.x * sensitivity;
    pitch -= mouseDelta.y * sensitivity;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    if (InputManager::instance().isKeyPressed(GLFW_KEY_W)) {
        camera.position += speed * front * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_S)) {
        camera.position -= speed * front * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_A)) {
        camera.position -= speed * right * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_D)) {
        camera.position += speed * right * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        camera.position -= speed * up * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_SPACE)) {
        camera.position += speed * up * dt;
    }

    camera.target = camera.position + glm::normalize(front);
}