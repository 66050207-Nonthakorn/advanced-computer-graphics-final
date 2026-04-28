#include "Main/MainScene.hpp"

#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Manager/InputManager.hpp"
#include "Engine/Texture/TextureManager.hpp"

#include "glfw/glfw3.h"

#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const std::string SCREEN_TITLE = "Advanced Computer Graphics Final";

float yaw = -90, pitch = 0, sensitivity = 0.2f;
float speed = 10.0f;

MainScene::MainScene() {
    // camera = Camera(60.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);
    // camera.position = {0.0f, 8.0f, 24.0f};
    // camera.target = {0.0f, 0.0f, 0.0f};

    // // IBL
    // cubemap.iblIrradianceMap = TextureManager::instance().get("night-sky-ibl-irradiance");
    // cubemap.iblPrefilterMap  = TextureManager::instance().get("night-sky-ibl-prefilter");
    // cubemap.iblBrdfLUT       = TextureManager::instance().get("brdf-lut-512");
    // cubemap.iblPrefilterMips = 7;

    // const int gridSize = 8;
    // const float spacing = 2.2f;
    // const float origin = -0.5f * (gridSize - 1) * spacing;
    // auto sphereMesh = MeshManager::instance().get("sphere");

    // for (int row = 0; row < gridSize; ++row) {
    //     for (int col = 0; col < gridSize; ++col) {
    //         SceneObject sphere;
    //         sphere.mesh = sphereMesh;
    //         sphere.transform.setPosition({
    //             origin + col * spacing,
    //             origin + row * spacing,
    //             0.0f
    //         });

    //         auto pbr = std::make_shared<PBRMaterial>(nullptr, nullptr, nullptr, nullptr, nullptr);
    //         pbr->metallicValue = static_cast<float>(col) / static_cast<float>(gridSize - 1);
    //         pbr->roughnessValue = static_cast<float>(row) / static_cast<float>(gridSize - 1);
    //         sphere.material = pbr;

    //         this->sceneObjects.emplace_back(std::move(sphere));
    //     }
    // }

    // PointLight light;
    // light.position = {0.0f, 8.0f, 8.0f};
    // light.color = {1.0f, 1.0f, 1.0f};
    // light.intensity = 200.0f;

    // this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    // this->directionalLight.color = glm::vec3(0.2f, 0.2f, 0.2f);
    // this->directionalLight.intensity = 1.0f;

    // this->lights.emplace_back(light);

    camera = Camera(60.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);

    // IBL
    cubemap.iblIrradianceMap = TextureManager::instance().get("night-sky-ibl-irradiance");
    cubemap.iblPrefilterMap  = TextureManager::instance().get("night-sky-ibl-prefilter");
    cubemap.iblBrdfLUT       = TextureManager::instance().get("brdf-lut-512");
    cubemap.iblPrefilterMips = 7;

    SceneObject object;
    object.mesh = MeshManager::instance().get("sphere");
    object.material = MaterialManager::instance().get("brick");

    SceneObject object2;
    object2.transform.setPosition({2, 1, 0});
    object2.mesh = MeshManager::instance().get("sphere");
    object2.material = MaterialManager::instance().get("marble");

    SceneObject object3;
    object3.transform.setPosition({4, 1, 0});
    object3.transform.setScale({10, 10, 10});
    object3.mesh = MeshManager::instance().getFromFile("Models/Bread.obj");
    object3.material = MaterialManager::instance().get("bread");

    SceneObject face;
    face.transform.setPosition({2, 2, 0});
    face.transform.setScale({.01, .01, .01});
    face.mesh = MeshManager::instance().getFromFile("Models/Caligula.obj");
    face.material = MaterialManager::instance().get("gold");

    SceneObject plane;
    plane.mesh = MeshManager::instance().get("plane");
    plane.material = MaterialManager::instance().get("metal");
    plane.transform.setPosition({0, -2, 0});
    plane.transform.setScale({10, 1, 10});

    PointLight light;
    light.position = {0.0, 2.5, 0.0};
    light.color = {1.0, 1.0, 1.0};
    light.intensity = 10.0f;

    this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    this->directionalLight.color = glm::vec3(0.2f, 0.2f, 0.2f);
    this->directionalLight.intensity = 1.0f;

    this->sceneObjects.emplace_back(object);
    this->sceneObjects.emplace_back(object2);
    this->sceneObjects.emplace_back(object3);
    this->sceneObjects.emplace_back(plane);
    this->sceneObjects.emplace_back(face);

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