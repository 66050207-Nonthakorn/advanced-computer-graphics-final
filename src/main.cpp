#include <GL/glew.h>

#include "Engine/Window.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Mesh/MeshManager.hpp"
#include "Engine/Shader/ShaderManager.hpp"
#include "Engine/Texture/TextureManager.hpp"
#include "Engine/Texture/HDRCubemapTexture.hpp"
#include "Engine/Material/MaterialManager.hpp"
#include "Engine/Manager/InputManager.hpp"
#include "Engine/Material/BlinnPhongMaterial.hpp"
#include "Engine/Material/PBRMaterial.hpp"
#include "Engine/Texture/Texture2D.hpp"
#include "Engine/Texture/BRDFLUTTexture.hpp"

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
    shaderManager.load("brdf", "Shaders/brdf.vert", "Shaders/brdf.frag");

    // Load texture
    textureManager.add("dirt", std::make_shared<Texture2D>("Textures/dirt.jpg"));
    textureManager.add("face", std::make_shared<Texture2D>("Textures/face.jpg"));

    // IBL — irradiance cubemap (diffuse)
    textureManager.add("sky-ibl-irradiance", std::make_shared<HDRCubemapTexture>(
        std::array<std::string, 6>{
            "Textures/Cubemap/Sky/px.hdr",
            "Textures/Cubemap/Sky/nx.hdr",
            "Textures/Cubemap/Sky/py.hdr",
            "Textures/Cubemap/Sky/ny.hdr",
            "Textures/Cubemap/Sky/pz.hdr",
            "Textures/Cubemap/Sky/nz.hdr"
        }
    ));

    // IBL — prefiltered specular cubemap (mip 0 = sharp, mip 3 = rough)
    textureManager.add("sky-ibl-prefilter", std::make_shared<HDRCubemapTexture>(
        std::vector<std::array<std::string, 6>>{
            { // mip 0
                "Textures/Cubemap/Sky/m0_px.hdr",
                "Textures/Cubemap/Sky/m0_nx.hdr",
                "Textures/Cubemap/Sky/m0_py.hdr",
                "Textures/Cubemap/Sky/m0_ny.hdr",
                "Textures/Cubemap/Sky/m0_pz.hdr",
                "Textures/Cubemap/Sky/m0_nz.hdr"
            },
            { // mip 1
                "Textures/Cubemap/Sky/m1_px.hdr",
                "Textures/Cubemap/Sky/m1_nx.hdr",
                "Textures/Cubemap/Sky/m1_py.hdr",
                "Textures/Cubemap/Sky/m1_ny.hdr",
                "Textures/Cubemap/Sky/m1_pz.hdr",
                "Textures/Cubemap/Sky/m1_nz.hdr"
            },
            { // mip 2
                "Textures/Cubemap/Sky/m2_px.hdr",
                "Textures/Cubemap/Sky/m2_nx.hdr",
                "Textures/Cubemap/Sky/m2_py.hdr",
                "Textures/Cubemap/Sky/m2_ny.hdr",
                "Textures/Cubemap/Sky/m2_pz.hdr",
                "Textures/Cubemap/Sky/m2_nz.hdr"
            },
            { // mip 3
                "Textures/Cubemap/Sky/m3_px.hdr",
                "Textures/Cubemap/Sky/m3_nx.hdr",
                "Textures/Cubemap/Sky/m3_py.hdr",
                "Textures/Cubemap/Sky/m3_ny.hdr",
                "Textures/Cubemap/Sky/m3_pz.hdr",
                "Textures/Cubemap/Sky/m3_nz.hdr"
            },
            { // mip 4
                "Textures/Cubemap/Sky/m4_px.hdr",
                "Textures/Cubemap/Sky/m4_nx.hdr",
                "Textures/Cubemap/Sky/m4_py.hdr",
                "Textures/Cubemap/Sky/m4_ny.hdr",
                "Textures/Cubemap/Sky/m4_pz.hdr",
                "Textures/Cubemap/Sky/m4_nz.hdr"
            }
        }
    ));

    textureManager.add("brdf-lut-512", std::make_shared<BRDFLUTTexture>(512));

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