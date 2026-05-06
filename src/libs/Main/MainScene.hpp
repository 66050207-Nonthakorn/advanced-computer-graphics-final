#pragma once

#include "Engine/Scene.hpp"
#include "Engine/Cloth/ClothMesh.hpp"

#include "MoveableCamera.hpp"
#include "DebugConsole.hpp"

#include <memory>

class MainScene : public Scene {
public:
    MainScene();
    void update(float dt) override;

private:
    std::shared_ptr<ClothMesh> cloth;
    std::shared_ptr<MovableCamera> movableCamera;
    float lightOrbitTime = 0.0f;

    DebugConsole debugConsole;
    std::vector<DebugConsole::Command> pendingCmds;

    std::vector<std::shared_ptr<SceneObject>> lightIcons;

    void applyDebugCommands();
};
