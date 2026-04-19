#pragma once

#include "Engine/Scene.hpp"

class MainScene : public Scene {
public:
    MainScene();
    void update(float dt) override;
};