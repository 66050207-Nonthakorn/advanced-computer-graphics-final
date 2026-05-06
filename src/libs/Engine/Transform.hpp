#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

class Transform {
public:
    Transform();

    // Local space
    glm::vec3 getPosition() const;
    glm::vec3 getRotation() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    glm::vec3 getForward();
    glm::vec3 getUp();
    glm::vec3 getRight();

    void setParent(Transform* parent);

    glm::mat4 getLocalMatrix();
    glm::mat4 getWorldMatrix();

    void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

private:
    bool isDirty;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 localMatrix;
    Transform* parent = nullptr;

    void recomputeLocal();
};