#include <GL/glew.h>

#include "Engine/Window.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Manager/MeshManager.hpp"
#include "Engine/Manager/ShaderManager.hpp"
#include "Engine/Manager/TextureManager.hpp"
#include "Engine/Manager/MaterialManager.hpp"
#include "Engine/Manager/InputManager.hpp"
#include "Engine/Material/BlinnPhongMaterial.hpp"
#include "Engine/Material/BillboardMaterial.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Material/ParticleMaterial.hpp"
#include "Engine/Texture/Texture2D.hpp"
#include "Engine/Texture/BRDFLUTTexture.hpp"
#include "Engine/Texture/HDREquirectangularTexture.hpp"
#include "Engine/Utils/StringUtils.hpp"

#include "Main/MainScene.hpp"
#include "Main/Constants.hpp"

#include "glm/vec3.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <thread>

static void loadAssets(const Window& window);

int main() {
    Window window(
        Constants::SCREEN_WIDTH,
        Constants::SCREEN_HEIGHT,
        Constants::SCREEN_TITLE
    );
    
    Renderer renderer(window);
    loadAssets(window);

    auto& inputManager = InputManager::instance();
    inputManager.init(window.getHandle());

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
            window.setIsShowCursor(!window.getIsShowCursor());
        }
        
        mainScene.update(dt);
        renderer.draw(mainScene);

        window.swapBuffers();
    }

    return 0;
}

static void loadAssets(const Window& window) {
    auto& shaderManager = ShaderManager::instance();
    auto& textureManager = TextureManager::instance();
    auto& materialManager = MaterialManager::instance();

    shaderManager.load("unlit", "Shaders/unlit.vert", "Shaders/unlit.frag");
    shaderManager.load("pbr", "Shaders/pbr.vert", "Shaders/pbr.frag");
    shaderManager.load("shadow", "Shaders/shadow.vert", "Shaders/shadow.frag");
    shaderManager.load("cubemap", "Shaders/cubemap.vert", "Shaders/cubemap.frag");
    shaderManager.load("cubemap-equirect", "Shaders/cubemap.vert", "Shaders/cubemap-equirect.frag");
    shaderManager.load("cubemap-irradiance", "Shaders/cubemap.vert", "Shaders/cubemap-irradiance.frag");
    shaderManager.load("cubemap-prefilter", "Shaders/cubemap.vert", "Shaders/cubemap-prefilter.frag");
    shaderManager.load("brdf", "Shaders/brdf.vert", "Shaders/brdf.frag");
    shaderManager.load("particle", "Shaders/particle.vert", "Shaders/particle.frag");
    shaderManager.load("billboard", "Shaders/billboard.vert", "Shaders/billboard.frag");

    textureManager.add("light-icon", std::make_shared<Texture2D>("Textures/light-bulb.png"));
    textureManager.add("pure-sky-hdr", std::make_shared<HDREquirectangularTexture>("Textures/Cubemap/PureSky.hdr"));
    textureManager.add("street-night-hdr", std::make_shared<HDREquirectangularTexture>("Textures/Cubemap/StreetNight.hdr"));
    textureManager.add("sunset-hdr", std::make_shared<HDREquirectangularTexture>("Textures/Cubemap/Sunset.hdr"));
    textureManager.add("brdf-lut-512", std::make_shared<BRDFLUTTexture>(512));
    textureManager.addPBR("table", "Textures/PBR/Table");
    textureManager.addPBR("marble", "Textures/PBR/Marble");
    textureManager.addPBR("glass", "Textures/PBR/Glass");
    textureManager.addPBR("cloth", "Textures/PBR/Cloth");
    textureManager.addPBR("macbook", "Textures/PBR/macbook");

    // Add Materials
    materialManager.add("light-icon", std::make_shared<BillboardMaterial>(textureManager.get("light-icon")));
    materialManager.add("particle", std::make_shared<ParticleMaterial>());
    materialManager.addPBR("default", "default");
    materialManager.addPBR("table", "table", 2.0f);
    materialManager.addPBR("marble", "marble");
    materialManager.addPBR("glass", "glass");
    materialManager.addPBR("cloth", "cloth", 2.0f);
    materialManager.addPBR("macbook", "macbook");
}