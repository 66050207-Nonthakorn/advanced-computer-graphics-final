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
    std::shared_ptr<MovableCamera> movableCamera;
    std::shared_ptr<ClothMesh> cloth;
    std::shared_ptr<ParticleMesh> cupSmokeParticle;
    bool pointLightAnimationEnabled = true;

    // Debug console
    std::shared_ptr<DebugConsole> debugConsole;
    std::vector<DebugConsole::Command> pendingCmds;
};