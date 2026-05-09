#include "MainScene.hpp"

#include "glfw/glfw3.h"

#include "Main/Constants.hpp"

#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Manager/MaterialManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Particle/ParticleMesh.hpp"

#include "glm/glm.hpp"

MainScene::MainScene() {
    // Camera
    movableCamera = std::make_shared<MovableCamera>(60.0f, Constants::ASPECT_RATIO, 0.1f, 100.0f);
    movableCamera->position = {0, 4.5f, 4.5f};
    movableCamera->speed = 10.0f;
    this->camera = movableCamera;
    this->sceneObjects.emplace_back(movableCamera);

    // Cubemap
    cubemap = HDRCubemap(
        TextureManager::instance().get("sunset-hdr"),
        TextureManager::instance().get("brdf-lut-512")
    );
    cubemap.iblIntensity = 0.075f;

    // Scene objects
    cupSmokeParticle = std::make_shared<ParticleMesh>(1000);
    cupSmokeParticle->emitRate     = 150.0f;
    cupSmokeParticle->gravity      = { 0.0f, -0.08f, 0.0f };
    cupSmokeParticle->minLife      = 3.5f;
    cupSmokeParticle->maxLife      = 5.5f;
    cupSmokeParticle->minSize      = 0.08f;
    cupSmokeParticle->maxSize      = 0.12f;
    cupSmokeParticle->minVelocity  = { -0.15f, 0.8f, -0.15f };
    cupSmokeParticle->maxVelocity  = {  0.15f, 1.3f,  0.15f };
    cupSmokeParticle->startColor   = { 0.28f, 0.28f, 0.30f, 0.20f };
    cupSmokeParticle->endColor     = { 0.78f, 0.78f, 0.80f, 0.0f };

    cloth = std::make_shared<ClothMesh>(100, 100, 12.0f, 4.5f);
    cloth->wind = { 1.0f, 0.0f, 1.0f }; 
    cloth->solverIterations = 15;
    cloth->damping = 0.99f;
    cloth->windGustAmplitude = 18.0f;
    
    cloth->pin(0);
    cloth->pin(99);

    this->sceneObjects.emplace_back(
        SceneObject::builder()
            .withMesh(MeshManager::instance().getFromFile("Models/room2.obj"))
            .withMaterial(MaterialManager::instance().get("marble"))
            .withRotation({0.0f, 180.0f, 0.0f})
            .withChild(
                SceneObject::builder()
                    .withMesh(MeshManager::instance().getFromFile("Models/table.obj"))
                    .withMaterial(MaterialManager::instance().get("table"))
                    .withPosition({-2.0f, 0.0f, -3.5f})
                    .withRotation({0.0f, 90.0f, 0.0f})
                    .withScale({0.2f, 0.2f, 0.2f})
                    .withChild(
                        SceneObject::builder()
                            .withMesh(MeshManager::instance().getFromFile("Models/Cup.obj"))
                            .withMaterial(MaterialManager::instance().get("glass"))
                            .withPosition({7.0f, 22.0f, 0.0f})
                            .withRotation({0.0f, 90.0f, 0.0f})
                            .withScale({30.0f, 30.0f, 30.0f})
                            .withChild(
                                SceneObject::builder()
                                    .withMesh(cupSmokeParticle)
                                    .withMaterial(MaterialManager::instance().get("particle"))
                                    .withPosition({0.0f, 0.0f, 0.0f})
                                    .withRotation({0.0f, -180.0f, 0.0f})
                                    .withScale({0.25f, 0.25f, 0.25f})
                                    .build()
                            )
                            .build()
                    )
                    .withChild(
                        SceneObject::builder()
                            .withMesh(MeshManager::instance().getFromFile("Models/macbook neo.obj"))
                            .withMaterial(MaterialManager::instance().get("macbook"))
                            .withRotation({0.0f, 180.0f, 0.0f})
                            .withPosition({-3.5f, 23.0f, 0.0f})
                            .withScale({50.0f, 50.0f, 50.0f})
                            .build()
                    )
                    .build()
            )
            .withChild(
                SceneObject::builder()
                    .withMesh(cloth)
                    .withMaterial(MaterialManager::instance().get("cloth"))
                    .withRotation({0.0f, -90.0f, 0.0f})
                    .withPosition({-11.0f, 10.0f, 0.0f})
                    .build()
            )
            .build()
    );

    // Lighting
    this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -0.5f, 0.5f));
    this->directionalLight.color     = glm::vec3(0.95f, 0.5f, 0.21f);
    this->directionalLight.intensity = 15.0f;

    PointLight light;
    light.position  = {0.0f, 10.0f, 10.0f};
    light.color     = {1.0f, 0.0f, 0.0f};
    light.intensity = 100.0f;
    this->lights.emplace_back(light);
    this->sceneObjects.emplace_back(
        SceneObject::builder()
            .withName("LightIcon")
            .withActive(false)
            .withMesh(MeshManager::instance().get("quad"))
            .withMaterial(MaterialManager::instance().get("light-icon"))
            .withPosition(light.position)
            .build()
    );

    PointLight light2;
    light2.position  = {5.0f, 10.0f, 5.0f};
    light2.color     = {0.85f, 0.95f, 1.0f};
    light2.intensity = 100.0f;
    this->lights.emplace_back(light2);
    this->sceneObjects.emplace_back(
        SceneObject::builder()
            .withName("LightIcon")
            .withActive(false)
            .withMesh(MeshManager::instance().get("quad"))
            .withMaterial(MaterialManager::instance().get("light-icon"))
            .withPosition(light2.position)
            .build()
    );
    
    // Debug console
    debugConsole = std::make_shared<DebugConsole>();
}

void MainScene::update(float dt) {
    debugConsole->poll(pendingCmds);
    applyDebugCommands();

    cloth->update(dt);
    cupSmokeParticle->update(dt, camera->getView());

    if (pointLightAnimationEnabled && !lights.empty()) {
        lights[0].intensity = 20.0f + 10.0f * std::sin(glfwGetTime() * 2.0f);
    }

    Scene::update(dt);
}

namespace {
    // Helper function to set debug mode for all PBR materials
    void setDebugMode(int mode) {
        static const char* pbrNames[] = { "default", "coffee-cup", "table", "marble", "glass" };
        for (auto& name : pbrNames) {
            auto mat = std::dynamic_pointer_cast<PBRMaterial>(MaterialManager::instance().get(name));
            if (mat) mat->debugMode = mode;
        }
    }
}

void MainScene::applyDebugCommands() {
    using C = DebugConsole::Command;

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

            case C::Type::PointLightDebug:
                for (auto& sceneObject : sceneObjects) {
                    if (sceneObject->name == "LightIcon") {
                        sceneObject->isActive = static_cast<bool>(cmd.f0);
                    }
                }
                break;
            case C::Type::PointLightIntensity:
                for (auto& light : lights) {
                    light.intensity = cmd.f0;
                }
                pointLightAnimationEnabled = false;
                break;
            case C::Type::PointLightColor:
                for (auto& light : lights) {
                    light.color = { cmd.f0, cmd.f1, cmd.f2 };
                }
                break;
            case C::Type::PointLightAnimate:
                pointLightAnimationEnabled = static_cast<bool>(cmd.f0);
                break;

            case C::Type::DirectionalLightDirection:
                directionalLight.direction = glm::normalize(glm::vec3(cmd.f0, cmd.f1, cmd.f2));
                break;
            case C::Type::DirectionalLightIntensity:
                directionalLight.intensity = cmd.f0;
                break;
            case C::Type::DirectionalLightColor:
                directionalLight.color = { cmd.f0, cmd.f1, cmd.f2 };
                break;

            case C::Type::CameraLock:
                movableCamera->isLocked = static_cast<bool>(cmd.f0);
                break;
            case C::Type::CameraReset:
                movableCamera->position = {0, 0, 3};
                movableCamera->target = {0, 0, 0};
                movableCamera->up = {0, 1, 0};
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
            
            case C::Type::ShadingNormal:   setDebugMode(0); break;
            case C::Type::ShadingShadow:   setDebugMode(1); break;
            case C::Type::ShadingAmbient:  setDebugMode(2); break;
            case C::Type::ShadingDiffuse:  setDebugMode(3); break;
            case C::Type::ShadingSpecular: setDebugMode(4); break;

            default: break;
        }
    }

    pendingCmds.clear();
}

