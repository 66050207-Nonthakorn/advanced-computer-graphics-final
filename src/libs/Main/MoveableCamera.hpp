#pragma once

#include "Engine/Camera.hpp"
#include "Engine/SceneObject.hpp"

class MovableCamera : public Camera, public SceneObject {
public:
    MovableCamera(float fovDegrees, float aspect, float nearPlane, float farPlane);
    void update(float dt) override;
    float sensitivity = 0.2f;
    float speed = 20.0f;
    
    bool isLocked = false;

private:
    float yaw = -90.0f;
    float pitch = 0.0f;
};