#include <GL/glew.h>

#include "Engine/Window.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Shader/ShaderManager.hpp"
#include "Engine/Texture/TextureManager.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Manager/InputManager.hpp"
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

    // Manager
    auto& meshManager = MeshManager::instance();
    auto& shaderManager = ShaderManager::instance();
    auto& textureManager = TextureManager::instance();
    auto& materialManager = MaterialManager::instance();
    auto& inputManager = InputManager::instance();

    inputManager.init(window.getHandle());

    // Load shader
    shaderManager.load("blinn-phong", "Shaders/blinn-phong.vert", "Shaders/blinn-phong.frag");
    shaderManager.load("pbr", "Shaders/pbr.vert", "Shaders/pbr.frag");
    shaderManager.load("shadow", "Shaders/shadow.vert", "Shaders/shadow.frag");

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

        if (InputManager::instance().isKeyPressedOnce(GLFW_KEY_ESCAPE)) {
            window.close();
        }
        
        mainScene.update(dt);
        renderer.draw(mainScene);

        window.swapBuffers();
    }

    return 0;
}