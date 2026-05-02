#pragma once

#include "Engine/Scene.hpp"
#include "Engine/Cloth/ClothMesh.hpp"

#include <memory>

class MainScene : public Scene {
public:
    MainScene();
    void update(float dt) override;

private:
    std::shared_ptr<ClothMesh> cloth;
};
