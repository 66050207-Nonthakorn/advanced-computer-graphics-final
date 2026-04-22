#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
public:
    Camera() = default;
    Camera(float fovDegrees, float aspect, float nearPlane, float farPlane);

    glm::mat4 getView() const;
    glm::mat4 getProjection() const;

    glm::vec3 position{0, 0, 3};
    glm::vec3 target{0, 0, 0};
    glm::vec3 up{0, 1, 0};

private:
    float fov{60.0f};
    float aspect{4.0f / 3.0f};
    float nearPlane{0.1f};
    float farPlane{100.0f};
};
