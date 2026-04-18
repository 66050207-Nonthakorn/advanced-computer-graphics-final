#include <GL/glew.h>

#include "Engine/Window.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Camera.hpp"

#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Shader/ShaderManager.hpp"

#include <vector>
#include <memory>
#include <iostream>

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

    // Load resource
    shaderManager.load("basic", "Shaders/basic.vert", "Shaders/basic.frag");

    // Object initialize
    auto object{std::make_shared<SceneObject>()};
    object->mesh = meshManager.get("sphere");
    object->material = std::make_shared<Material>(shaderManager.get("basic"));

    // Objects
    std::vector<std::shared_ptr<SceneObject>> objects{ object };

    float yaw = -90, pitch = 0, sensitivity = 0.2f;
    float speed = 0.05f;

    while (!window.shouldClose()) {
        window.pollEvent();
        renderer.clearScreen();

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
            camera.position += speed * front;
        }
        if (window.isKeyPressed(GLFW_KEY_S)) {
            camera.position -= speed * front;
        }
        if (window.isKeyPressed(GLFW_KEY_A)) {
            camera.position -= speed * right;
        }
        if (window.isKeyPressed(GLFW_KEY_D)) {
            camera.position += speed * right;
        }
        if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            camera.position -= speed * up;
        }
        if (window.isKeyPressed(GLFW_KEY_SPACE)) {
            camera.position += speed * up;
        }

        camera.target = camera.position + glm::normalize(front);

        renderer.draw(objects, camera);
        window.swapBuffers();
    }

    return 0;
}