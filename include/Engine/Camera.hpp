#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
public:
    Camera(float fovDegrees, float aspect, float nearPlane, float farPlane);

    glm::mat4 getView() const;
    glm::mat4 getProjection() const;

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

private:
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;
};
