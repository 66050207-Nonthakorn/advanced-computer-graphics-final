#pragma once

#include "Engine/Scene.hpp"

class PBRTestScene : public Scene {
public:
    PBRTestScene();
    void update(float dt) override;
};