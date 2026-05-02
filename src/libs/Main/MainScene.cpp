#include "Main/MainScene.hpp"
#include "Main/Constanst.hpp"
#include "Main/MoveableCamera.hpp"
#include "Main/DebugConsole.hpp"

#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Manager/MaterialManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Manager/InputManager.hpp"
#include "Engine/Manager/TextureManager.hpp"

#include "glfw/glfw3.h"

#include <cmath>
#include <filesystem>
#include <iostream>

MainScene::MainScene() {
    float aspect = static_cast<float>(Constanst::SCREEN_WIDTH) / Constanst::SCREEN_HEIGHT;

    movableCamera = std::make_shared<MovableCamera>(60.0f, aspect, 0.1f, 100.0f);
    this->camera = movableCamera;

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
    face->transform.setPosition({ 2, 2, 0});
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

    // Cloth — 30x30 grid, 4x4 world units, pinned at top two corners
    cloth = std::make_shared<ClothMesh>(30, 30, 4.0f, 4.0f);
    cloth->wind = { 1.0f, 0.0f, 1.0f };
    cloth->windGustAmplitude = 4.0f;
    
    auto clothObj = std::make_shared<SceneObject>();
    clothObj->mesh = cloth;
    clothObj->material = MaterialManager::instance().get("cloth");
    clothObj->transform.setPosition({-4, 4, 0});

    PointLight light;
    light.position = {0.0, 2.5, 0.0};
    light.color = {1.0, 1.0, 1.0};
    light.intensity = 20.0f;

    this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    this->directionalLight.color = glm::vec3(0.2f, 0.2f, 0.2f);
    this->directionalLight.intensity = 10.0f;

    this->sceneObjects.emplace_back(object);
    this->sceneObjects.emplace_back(object2);
    this->sceneObjects.emplace_back(object3);
    this->sceneObjects.emplace_back(plane);
    this->sceneObjects.emplace_back(face);
    this->sceneObjects.emplace_back(face2);
    this->sceneObjects.emplace_back(clothObj);
    this->sceneObjects.emplace_back(movableCamera);

    this->lights.emplace_back(light);
}

void MainScene::applyDebugCommands() {
    using C = DebugConsole::Command;
    debugConsole.poll(pendingCmds);

    for (auto& cmd : pendingCmds) {
        switch (cmd.type) {
            case C::Type::ClothWind:
                cloth->wind = { cmd.f0, cmd.f1, cmd.f2 };
                break;
            case C::Type::ClothWindGust:
                cloth->windGustAmplitude = cmd.f0;
                break;
            case C::Type::ClothDamping:
                cloth->damping = cmd.f0;
                break;
            case C::Type::ClothGravity:
                cloth->gravity = { cmd.f0, cmd.f1, cmd.f2 };
                break;
            case C::Type::ClothIterations:
                cloth->solverIterations = static_cast<int>(cmd.f0);
                break;

            case C::Type::LightPos:
                if (!lights.empty())
                    lights[0].position = { cmd.f0, cmd.f1, cmd.f2 };
                break;
            case C::Type::LightIntensity:
                if (!lights.empty())
                    lights[0].intensity = cmd.f0;
                break;
            case C::Type::LightColor:
                if (!lights.empty())
                    lights[0].color = { cmd.f0, cmd.f1, cmd.f2 };
                break;

            case C::Type::DirLightDir:
                directionalLight.direction = glm::normalize(glm::vec3(cmd.f0, cmd.f1, cmd.f2));
                break;
            case C::Type::DirLightIntensity:
                directionalLight.intensity = cmd.f0;
                break;

            case C::Type::DrawFull:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case C::Type::DrawLine:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case C::Type::DrawPoint:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;

            default: break;
        }
    }

    pendingCmds.clear();
}

void MainScene::update(float dt) {
    applyDebugCommands();
    Scene::update(dt);

    if (InputManager::instance().isKeyPressed(GLFW_KEY_UP)) {
        cloth->windGustAmplitude += 0.1;
    }
    else if (InputManager::instance().isKeyPressed(GLFW_KEY_DOWN)) {
        cloth->windGustAmplitude -= 0.1;
    }

    cloth->update(dt);

    lightOrbitTime += dt;

    constexpr float orbitRadius = 5.0f;
    constexpr float orbitSpeed = 1.0f;
    constexpr float lightHeight = 2.5f;

    this->lights[0].position.x = orbitRadius * std::cos(lightOrbitTime * orbitSpeed);
    this->lights[0].position.y = lightHeight;
    this->lights[0].position.z = orbitRadius * std::sin(lightOrbitTime * orbitSpeed);
}