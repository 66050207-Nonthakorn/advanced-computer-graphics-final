#include "MainScene.hpp"

#include "glfw/glfw3.h"

#include "Main/Constants.hpp"

#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Manager/MaterialManager.hpp"
#include "Engine/Mesh/MeshData.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Particle/ParticleMesh.hpp"

#include "glm/glm.hpp"

#include <iostream>

MainScene::MainScene() {
    // Camera
    movableCamera = std::make_shared<MovableCamera>(60.0f, Constants::ASPECT_RATIO, 0.1f, 100.0f);
    movableCamera->position = Constants::CAMERA_START_POS;
    movableCamera->target = Constants::CAMERA_START_TARGET;
    movableCamera->speed = 10.0f;
    movableCamera->isLocked = true;
    this->camera = movableCamera;
    this->sceneObjects.emplace_back(movableCamera);

    // Cubemap
    cubemap = HDRCubemap(
        TextureManager::instance().get("sunset-hdr"),
        TextureManager::instance().get("brdf-lut-512")
    );
    cubemap.iblIntensity = Constants::IBL_INTENSITY;

    // Scene objects
    cupSmokeParticle = std::make_shared<ParticleMesh>(1000);
    cupSmokeParticle->emitRate     = Constants::PARTICLE_EMIT_RATE;
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
    cloth->wind = { 0.5f, 0.0f, 1.0f }; 
    cloth->solverIterations = 20;
    cloth->damping = 0.99f;
    cloth->windGustAmplitude = 15.0f;
    
    cloth->pin(0);
    cloth->pin(49);
    cloth->pin(99);

    this->sceneObjects.emplace_back(
        SceneObject::builder()
            .withMesh(MeshManager::instance().getFromFile("Models/Room.obj"))
            .withMaterial(MaterialManager::instance().get("marble"))
            .withRotation({0.0f, 180.0f, 0.0f})
            .withChild(
                SceneObject::builder()
                    .withMesh(MeshManager::instance().getFromFile("Models/Table.obj"))
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
                            .withMesh(MeshManager::instance().getFromFile("Models/Macbook Neo.obj"))
                            .withMaterial(MaterialManager::instance().get("macbook"))
                            .withRotation({0.0f, 180.0f, 0.0f})
                            .withPosition({-3.5f, 22.5f, 0.0f})
                            .withScale({50.0f, 50.0f, 50.0f})
                            .build()
                    )
                    .withChild(
                        SceneObject::builder()
                            .withMesh(MeshManager::instance().getFromFile("Models/Lamp.obj"))
                            .withMaterial(MaterialManager::instance().get("glass"))
                            .withPosition({6.0f, 22.0f, -7.0f})
                            .withRotation({0.0f, 200.0f, 0.0f})
                            .withScale({16.0f, 16.0f, 16.0f})
                            .build()
                    )
                    .build()
            )
            .withChild(
                SceneObject::builder()
                    .withMesh(cloth)
                    .withMaterial(MaterialManager::instance().get("cloth"))
                    .withRotation({0.0f, 90.0f, 0.0f})
                    .withPosition({-10.0f, 7.5f, 0.0f})
                    .build()
            )
            .build()
    );

    for (auto& obj : this->sceneObjects) {
        createNormalDebugObjects(obj);
    }

    // Lighting
    this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -0.5f, 0.5f));
    this->directionalLight.color     = glm::vec3(0.95f, 0.5f, 0.21f);
    this->directionalLight.intensity = Constants::DIRECTIONAL_INTENSITY;

    lampLight = std::make_shared<PointLight>();
    lampLight->position  = {2.6f, 6.25f, 2.7f};
    lampLight->color     = {0.85f, 0.95f, 1.0f};
    lampLight->intensity = Constants::POINTLIGHT_INTENSITY;
    lampLight->range     = 5.0f;
    this->lights.emplace_back(lampLight);
    auto lampIcon = SceneObject::builder()
        .withActive(false)
        .withMesh(MeshManager::instance().get("quad"))
        .withMaterial(MaterialManager::instance().get("light-icon"))
        .withPosition(lampLight->position)
        .build();
    this->pointLightIcons.emplace_back(lampIcon);
    this->sceneObjects.emplace_back(lampIcon);

    roomLight = std::make_shared<PointLight>();
    roomLight->position  = {0.0f, 10.5f, 0.0f};
    roomLight->color     = {1.0f, 0.96f, 0.1f};
    roomLight->intensity = Constants::POINTLIGHT_INTENSITY;
    roomLight->range     = 20.0f;
    this->lights.emplace_back(roomLight);
    auto roomIcon = SceneObject::builder()
        .withActive(false)
        .withMesh(MeshManager::instance().get("quad"))
        .withMaterial(MaterialManager::instance().get("light-icon"))
        .withPosition(roomLight->position)
        .build();
    this->pointLightIcons.emplace_back(roomIcon);
    this->sceneObjects.emplace_back(roomIcon);

    // Debug console
    debugConsole = std::make_shared<DebugConsole>();
}   

void MainScene::update(float dt) {
    debugConsole->poll(pendingCmds);
    applyDebugCommands();

    for (int i = 0; i < pointLightIcons.size(); i++) {
        if (pointLightIcons[i] && lights[i]) {
            pointLightIcons[i]->transform.setPosition(lights[i]->position);
        }
    }

    cupSmokeParticle->update(dt, camera->getView());
    cloth->update(dt);
    if (normalDebugEnabled) {
        updateClothNormalDebugMesh();
    }

    if (lightsEnabled && pointLightAnimationEnabled && !lights.empty()) {
        // Light effect that is like electricity is crackling and flickering
        float time = static_cast<float>(glfwGetTime());
        lampLight->intensity = 40.0f + 10.0f * std::sin(time * 20.0f) + 10.0f * std::sin(time * 35.0f);

        // Room light moving in a circular path
        float radius = 5.0f;
        roomLight->position.x = radius * std::cos(time * 0.5f);
        roomLight->position.z = radius * std::sin(time * 0.5f);
    }
    else if (!lights.empty()) {
        lampLight->intensity = lightsEnabled ? Constants::POINTLIGHT_INTENSITY : 0.0f;
        roomLight->intensity = lightsEnabled ? Constants::POINTLIGHT_INTENSITY : 0.0f;
    }

    Scene::update(dt);
}

namespace {

void setDebugMode(int mode) {
    static const char* pbrNames[] = { "default", "table", "marble", "glass", "cloth", "macbook" };
    for (auto& name : pbrNames) {
        auto mat = std::dynamic_pointer_cast<PBRMaterial>(MaterialManager::instance().get(name));
        if (mat) mat->debugMode = mode;
    }
}

std::shared_ptr<Mesh> createNormalLineMesh(const std::shared_ptr<Mesh>& sourceMesh, float length = 0.2f) {
    MeshData lineData;
    lineData.vertices.reserve(sourceMesh->vertices.size() * 2);
    lineData.indices.reserve(sourceMesh->vertices.size() * 2);

    for (const auto& v : sourceMesh->vertices) {
        if (glm::length(v.normal) < 1e-5f) continue;

        const glm::vec3 normal = glm::normalize(v.normal);
        const glm::vec3 p0 = v.position;
        const glm::vec3 p1 = v.position + normal * length;

        const unsigned int base = static_cast<unsigned int>(lineData.vertices.size());
        lineData.vertices.push_back({ p0, normal, {0.0f, 0.0f} });
        lineData.vertices.push_back({ p1, normal, {1.0f, 0.0f} });
        lineData.indices.push_back(base);
        lineData.indices.push_back(base + 1);
    }

    if (lineData.indices.empty()) return nullptr;
    return std::make_shared<Mesh>(lineData, 0x0001);
}

}

void MainScene::createNormalDebugObjects(const std::shared_ptr<SceneObject>& node) {
    if (!node) return;

    if (node->mesh != nullptr &&
        dynamic_cast<ClothMesh*>(node->mesh.get()) != nullptr
    ) {
        auto lineMesh = createNormalLineMesh(node->mesh);
        if (lineMesh) {
            auto normalObj = SceneObject::builder()
                .withName(node->name + "_normal_debug")
                .withActive(false)
                .withMesh(lineMesh)
                .withMaterial(MaterialManager::instance().get("normal-debug"))
                .build();

            node->addChild(normalObj);
            clothNormalMesh = lineMesh;
            normalDebugObjects.emplace_back(normalObj);
        }
    }

    for (auto& child : node->children) {
        createNormalDebugObjects(child);
    }
}

void MainScene::updateClothNormalDebugMesh() {
    if (!cloth || !clothNormalMesh) return;

    const auto& src = cloth->vertices;
    auto& dst = clothNormalMesh->vertices;
    if (src.empty() || dst.size() != src.size() * 2) return;

    constexpr float normalLength = 0.2f;

    for (size_t i = 0; i < src.size(); ++i) {
        glm::vec3 n = src[i].normal;
        if (glm::length(n) > 1e-6f) {
            n = glm::normalize(n);
        }
        else {
            n = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        const glm::vec3 p0 = src[i].position;
        const glm::vec3 p1 = p0 + n * normalLength;

        dst[i * 2 + 0].position = p0;
        dst[i * 2 + 0].normal = n;
        dst[i * 2 + 1].position = p1;
        dst[i * 2 + 1].normal = n;
    }

    glBindBuffer(GL_ARRAY_BUFFER, clothNormalMesh->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    dst.size() * sizeof(Vertex),
                    dst.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MainScene::applyDebugCommands() {
    for (auto& cmd : pendingCmds) {
        if (cmd.cmd == "cloth") {
            if (cmd.arg == "normal") {
                normalDebugEnabled = (cmd.s0 == "on");
                for (auto& normalObj : normalDebugObjects) {
                    if (normalObj) normalObj->isActive = normalDebugEnabled;
                }
            }
            else if (cmd.arg == "wind") {
                cloth->windGustAmplitude = cmd.f0;
            }
        }
        else if (cmd.cmd == "light") {
            if (cmd.arg == "on") {
                lightsEnabled = true;
                directionalLight.intensity = Constants::DIRECTIONAL_INTENSITY;
                for (auto& light : lights) {
                    if (light) light->intensity = Constants::POINTLIGHT_INTENSITY;
                }
            }
            else if (cmd.arg == "off") {
                lightsEnabled = false;
                directionalLight.intensity = 0.0f;
                for (auto& light : lights) {
                    if (light) light->intensity = 0.0f;
                }
            }
        }
        else if (cmd.cmd == "ibl") {
            if (cmd.arg == "on") {
                cubemap.iblIntensity = Constants::IBL_INTENSITY;
            }
            else if (cmd.arg == "off") {
                cubemap.iblIntensity = 0.0f;
            }
        }
        else if (cmd.cmd == "pointlight") {
            if (cmd.arg == "icon") {
                for (auto& icon : pointLightIcons) {
                    if (icon) icon->isActive = (cmd.s0 == "on");
                }
            }
            else if (cmd.arg == "animate") {
                pointLightAnimationEnabled = (cmd.s0 == "on");
            }
        }
        else if (cmd.cmd == "particle") {
            if (cmd.arg == "on") {
                particleEnabled = true;
                cupSmokeParticle->emitRate = Constants::PARTICLE_EMIT_RATE;
            }
            else if (cmd.arg == "off") {
                particleEnabled = false;
                cupSmokeParticle->emitRate = 0.0f;
            }
        }
        else if (cmd.cmd == "camera") {
            if (cmd.arg == "lock") {
                movableCamera->isLocked = true;
            }
            else if (cmd.arg == "unlock") {
                movableCamera->isLocked = false;
            }
            else if (cmd.arg == "reset") {
                movableCamera->isLocked = true;
                movableCamera->position = Constants::CAMERA_START_POS;
                movableCamera->target = Constants::CAMERA_START_TARGET;
            }
        }
        else if (cmd.cmd == "draw") {
            if (cmd.arg == "full") {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else if (cmd.arg == "line") {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else if (cmd.arg == "point") {
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            }
        }
        else if (cmd.cmd == "shading") {
            if (cmd.arg == "full")        setDebugMode(0);
            else if (cmd.arg == "shadow") setDebugMode(1);
            else if (cmd.arg == "amb")    setDebugMode(2);
            else if (cmd.arg == "diff")   setDebugMode(3);
            else if (cmd.arg == "spec")   setDebugMode(4);
        } 
    }

    pendingCmds.clear();
}

