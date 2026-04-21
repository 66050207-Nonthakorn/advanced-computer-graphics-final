#include <GL/glew.h>

#include "Engine/Window.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Shader/ShaderManager.hpp"
#include "Engine/Texture/TextureManager.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Material/BlinnPhongMaterial.hpp"
#include "Engine/Material/PBRMaterial.hpp"

#include "Main/MainScene.hpp"

#include "glm/vec3.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <thread>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const std::string SCREEN_TITLE = "Advanced Computer Graphics Final";

int main() {
    Window window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
    window.setIsShowCursor(false);
    
    Renderer renderer;
    Camera camera(60.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);

    // Manager
    auto& meshManager = MeshManager::instance();
    auto& shaderManager = ShaderManager::instance();
    auto& textureManager = TextureManager::instance();
    auto& materialManager = MaterialManager::instance();

    // Load shader
    shaderManager.load("blinn-phong", "Shaders/blinn-phong.vert", "Shaders/blinn-phong.frag");
    shaderManager.load("pbr", "Shaders/pbr.vert", "Shaders/pbr.frag");

    // Load texture
    textureManager.loadAsync("dirt", "Textures/dirt.jpg");
    textureManager.loadPBRAsync("cloth",  "Textures/Cloth");
    textureManager.loadPBRAsync("silver", "Textures/Silver");
    textureManager.loadPBRAsync("brass",  "Textures/Brass");
    textureManager.loadPBRAsync("marble",  "Textures/Marble");
    textureManager.loadPBRAsync("boulders",  "Textures/Boulders");
    textureManager.finalize();

    // Add Materials
    materialManager.add("dirt", std::make_shared<BlinnPhongMaterial>(textureManager.get("dirt")));
    materialManager.addPBR("cloth", "cloth");
    materialManager.addPBR("silver", "silver");
    materialManager.addPBR("brass", "brass", 2.0f);
    materialManager.addPBR("marble", "marble");
    materialManager.addPBR("boulders", "boulders", 0.5f);

    MainScene mainScene;

    float yaw = -90, pitch = 0, sensitivity = 0.2f;
    float speed = 10.0f;

    float pt = glfwGetTime();
    float lastTime = pt;
    int frameCount = 0;

    while (!window.shouldClose()) {
        window.pollEvent();
        renderer.clearScreen();

        float t = glfwGetTime();
        float currentTime = t;
        float dt = t - pt;
        pt = t;
        frameCount++;

        if (currentTime - lastTime >= 0.1f) {
            double fps = frameCount / (currentTime - lastTime);

            std::cout << "FPS: " << fps << std::endl;

            frameCount = 0;
            lastTime = currentTime;
        }

        if (window.isKeyPressedOnce(GLFW_KEY_ESCAPE)) {
            window.close();
        }

        glm::vec2 mouseDelta = window.getMouseInput();
        yaw += mouseDelta.x * sensitivity;
        pitch -= mouseDelta.y * sensitivity;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        if (window.isKeyPressed(GLFW_KEY_W)) {
            camera.position += speed * front * dt;
        }
        if (window.isKeyPressed(GLFW_KEY_S)) {
            camera.position -= speed * front * dt;
        }
        if (window.isKeyPressed(GLFW_KEY_A)) {
            camera.position -= speed * right * dt;
        }
        if (window.isKeyPressed(GLFW_KEY_D)) {
            camera.position += speed * right * dt;
        }
        if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            camera.position -= speed * up * dt;
        }
        if (window.isKeyPressed(GLFW_KEY_SPACE)) {
            camera.position += speed * up * dt;
        }

        camera.target = camera.position + glm::normalize(front);
        
        mainScene.update(dt);
        renderer.draw(mainScene, camera);

        window.swapBuffers();
    }

    return 0;
}