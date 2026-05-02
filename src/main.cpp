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
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Texture/Texture2D.hpp"
#include "Engine/Texture/BRDFLUTTexture.hpp"
#include "Engine/Texture/HDREquirectangularTexture.hpp"
#include "Engine/Utils/StringUtils.hpp"

#include "Main/MainScene.hpp"
#include "Main/PBRTestScene.hpp"
#include "Main/Constanst.hpp"

#include "glm/vec3.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <thread>

static void loadAssets(const Window& window);

int main() {
    Window window(
        Constanst::SCREEN_WIDTH,
        Constanst::SCREEN_HEIGHT,
        Constanst::SCREEN_TITLE
    );
    // window.setIsShowCursor(false);
    Renderer renderer(window);
    loadAssets(window);

    MainScene mainScene;
    // PBRTestScene pbrTestScene;

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

        // pbrTestScene.update(dt);
        // renderer.draw(pbrTestScene);

        window.swapBuffers();
    }

    return 0;
}

static void loadAssets(const Window& window) {
    auto& shaderManager = ShaderManager::instance();
    auto& textureManager = TextureManager::instance();
    auto& materialManager = MaterialManager::instance();
    auto& inputManager = InputManager::instance();

    inputManager.init(window.getHandle());

    shaderManager.load("blinn-phong", "Shaders/blinn-phong.vert", "Shaders/blinn-phong.frag");
    shaderManager.load("pbr", "Shaders/pbr.vert", "Shaders/pbr.frag");
    shaderManager.load("shadow", "Shaders/shadow.vert", "Shaders/shadow.frag");
    shaderManager.load("cubemap", "Shaders/cubemap.vert", "Shaders/cubemap.frag");
    shaderManager.load("cubemap-equirect", "Shaders/cubemap.vert", "Shaders/cubemap-equirect.frag");
    shaderManager.load("cubemap-irradiance", "Shaders/cubemap.vert", "Shaders/cubemap-irradiance.frag");
    shaderManager.load("cubemap-prefilter", "Shaders/cubemap.vert", "Shaders/cubemap-prefilter.frag");
    shaderManager.load("brdf", "Shaders/brdf.vert", "Shaders/brdf.frag");

    textureManager.add("dirt", std::make_shared<Texture2D>("Textures/dirt.jpg"));
    textureManager.add("face", std::make_shared<Texture2D>("Textures/face.jpg"));    
    textureManager.add("night-sky-hdr",std::make_shared<HDREquirectangularTexture>("Textures/Cubemap/NightSky/night.hdr"));
    textureManager.add("brdf-lut-512", std::make_shared<BRDFLUTTexture>(512));

    // PBR Textures
    textureManager.addPBR("cloth",  "Textures/PBR/Cloth");
    textureManager.addPBR("silver", "Textures/PBR/Silver");
    textureManager.addPBR("brass",  "Textures/PBR/Brass");
    textureManager.addPBR("marble",  "Textures/PBR/Marble");
    textureManager.addPBR("boulders",  "Textures/PBR/Boulders");
    textureManager.addPBR("metal",  "Textures/PBR/Metal");
    textureManager.addPBR("gold",  "Textures/PBR/Gold");
    textureManager.addPBR("bread",  "Textures/PBR/Bread");
    textureManager.addPBR("brick",  "Textures/PBR/Brick");
    textureManager.addPBR("titanium",  "Textures/PBR/Titanium");

    // Add Materials
    materialManager.add("dirt", std::make_shared<BlinnPhongMaterial>(textureManager.get("dirt")));
    materialManager.add("face", std::make_shared<BlinnPhongMaterial>(textureManager.get("face")));
    materialManager.addPBR("default", "default");
    materialManager.addPBR("cloth", "cloth", 2.0f);
    materialManager.addPBR("silver", "silver");
    materialManager.addPBR("brass", "brass", 2.0f);
    materialManager.addPBR("marble", "marble");
    materialManager.addPBR("boulders", "boulders", 0.5f);
    materialManager.addPBR("metal", "metal");
    materialManager.addPBR("gold", "gold");
    materialManager.addPBR("bread", "bread", 2.0f);
    materialManager.addPBR("brick", "brick", 2.0f);
    materialManager.addPBR("titanium", "titanium");
}