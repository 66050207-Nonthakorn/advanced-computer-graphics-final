#pragma once

#include "Engine/Scene.hpp"
#include "Engine/Particle/ParticleMesh.hpp"
#include "Main/DebugConsole.hpp"
#include "Main/MoveableCamera.hpp"

#include <memory>

class MainScene : public Scene {
public:
    MainScene();
    void update(float dt) override;
    void applyDebugCommands();

private:
    void createNormalDebugObjects(const std::shared_ptr<SceneObject>& node);
    void updateClothNormalDebugMesh();

    std::shared_ptr<MovableCamera> movableCamera;
    
    std::shared_ptr<ClothMesh> cloth;
    std::shared_ptr<Mesh> clothNormalMesh;
    
    std::shared_ptr<ParticleMesh> cupSmokeParticle;
    
    std::shared_ptr<PointLight> lampLight;
    std::shared_ptr<PointLight> roomLight;

    std::vector<std::shared_ptr<SceneObject>> pointLightIcons;
    std::vector<std::shared_ptr<SceneObject>> normalDebugObjects;
    
    bool lightsEnabled = true;
    bool pointLightAnimationEnabled = true;
    bool particleEnabled = true;
    bool normalDebugEnabled = false;

    // Debug console
    std::shared_ptr<DebugConsole> debugConsole;
    std::vector<DebugConsole::Command> pendingCmds;
};