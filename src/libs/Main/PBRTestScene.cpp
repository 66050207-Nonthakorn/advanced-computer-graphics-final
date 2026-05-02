#include "PBRTestScene.hpp"
#include "Constanst.hpp"

#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Manager/InputManager.hpp"

namespace {

float yaw = -90, pitch = 0, sensitivity = 0.2f;
float speed = 10.0f;

}

PBRTestScene::PBRTestScene() {
    float aspect = static_cast<float>(Constanst::SCREEN_WIDTH) / Constanst::SCREEN_HEIGHT;
    camera = std::make_shared<Camera>(60.0f, aspect, 0.1f, 100.0f);
    camera->position = {0.0f, 8.0f, 24.0f};
    camera->target = {0.0f, 0.0f, 0.0f};

    cubemap = HDRCubemap(
        TextureManager::instance().get("night-sky-hdr"),
        TextureManager::instance().get("brdf-lut-512")
    );

    const int gridSize = 8;
    const float spacing = 2.2f;
    const float origin = -0.5f * (gridSize - 1) * spacing;
    auto sphereMesh = MeshManager::instance().get("sphere");

    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            auto sphere = std::make_shared<SceneObject>();
            sphere->mesh = sphereMesh;
            sphere->transform.setPosition({
                origin + col * spacing,
                origin + row * spacing,
                0.0f
            });

            auto pbr = std::make_shared<PBRMaterial>(nullptr, nullptr, nullptr, nullptr, nullptr);
            pbr->metallicValue = static_cast<float>(col) / static_cast<float>(gridSize - 1);
            pbr->roughnessValue = static_cast<float>(row) / static_cast<float>(gridSize - 1);
            sphere->material = pbr;

            this->sceneObjects.push_back(sphere);
        }
    }

    PointLight light;
    light.position = {0.0f, 8.0f, 8.0f};
    light.color = {1.0f, 1.0f, 1.0f};
    light.intensity = 200.0f;

    this->directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    this->directionalLight.color = glm::vec3(0.2f, 0.2f, 0.2f);
    this->directionalLight.intensity = 1.0f;

    this->lights.emplace_back(light);
}

void PBRTestScene::update(float dt) {
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
        camera->position += speed * front * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_S)) {
        camera->position -= speed * front * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_A)) {
        camera->position -= speed * right * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_D)) {
        camera->position += speed * right * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        camera->position -= speed * up * dt;
    }
    if (InputManager::instance().isKeyPressed(GLFW_KEY_SPACE)) {
        camera->position += speed * up * dt;
    }

    camera->target = camera->position + glm::normalize(front);
}
