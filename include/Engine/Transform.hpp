#pragma once

#include "glm/glm.hpp"

class Transform {
public:
    Transform();
    glm::mat4 getModel();

    glm::vec3 getPosition() const;
    glm::vec3 getRotation() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

private:
    bool isDirty;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 model;
    void recomputeModel();
};