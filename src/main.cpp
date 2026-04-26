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
#include "Engine/Texture/Texture2D.hpp"

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
    shaderManager.load("cubemap", "Shaders/cubemap.vert", "Shaders/cubemap.frag");

    // Load texture
    textureManager.add("dirt", std::make_shared<Texture2D>("Textures/dirt.jpg"));
    textureManager.add("face", std::make_shared<Texture2D>("Textures/face.jpg"));

    textureManager.add("room-cubemap", std::make_shared<CubemapTexture>(
        std::array<std::string, 6>{
            "Textures/Cubemap/Room/px.png",
            "Textures/Cubemap/Room/nx.png",
            "Textures/Cubemap/Room/py.png",
            "Textures/Cubemap/Room/ny.png",
            "Textures/Cubemap/Room/pz.png",
            "Textures/Cubemap/Room/nz.png"
        }
    ));

    textureManager.addPBR("cloth",  "Textures/Cloth");
    textureManager.addPBR("silver", "Textures/Silver");
    textureManager.addPBR("brass",  "Textures/Brass");
    textureManager.addPBR("marble",  "Textures/Marble");
    textureManager.addPBR("boulders",  "Textures/Boulders");
    textureManager.addPBR("metal",  "Textures/Metal");

    // Add Materials
    materialManager.add("dirt", std::make_shared<BlinnPhongMaterial>(textureManager.get("dirt")));
    materialManager.add("face", std::make_shared<BlinnPhongMaterial>(textureManager.get("face")));
    materialManager.addPBR("default", "default");
    materialManager.addPBR("cloth", "cloth");
    materialManager.addPBR("silver", "silver");
    materialManager.addPBR("brass", "brass", 2.0f);
    materialManager.addPBR("marble", "marble");
    materialManager.addPBR("boulders", "boulders", 0.5f);
    materialManager.addPBR("metal", "metal");

    MainScene mainScene;

    float pt = glfwGetTime();
    float lastTime = pt;

    while (!window.shouldClose()) {
        window.pollEvent();
        renderer.clearScreen();

        float t = glfwGetTime();
        float dt = t - pt;
        pt = t;

        if (InputManager::instance().isKeyPressedOnce(GLFW_KEY_ESCAPE)) {
            window.close();
        }
        
        mainScene.update(dt);
        renderer.draw(mainScene);

        window.swapBuffers();
    }

    return 0;
}