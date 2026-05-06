#include "Engine/Transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

Transform::Transform()
    : isDirty(true), position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), localMatrix(1.0f) {}

glm::vec3 Transform::getPosition() const { return this->position; }
glm::vec3 Transform::getRotation() const { return this->rotation; }
glm::vec3 Transform::getScale()    const { return this->scale; }

void Transform::setPosition(const glm::vec3& position) { isDirty = true; this->position = position; }
void Transform::setRotation(const glm::vec3& rotation) { isDirty = true; this->rotation = rotation; }
void Transform::setScale(const glm::vec3& scale)       { isDirty = true; this->scale = scale; }

glm::vec3 Transform::getForward() { return glm::normalize(glm::vec3(getWorldMatrix()[2])); }
glm::vec3 Transform::getUp()      { return glm::normalize(glm::vec3(getWorldMatrix()[1])); }
glm::vec3 Transform::getRight()   { return glm::normalize(glm::vec3(getWorldMatrix()[0])); }

void Transform::setParent(Transform* p) { parent = p; }

glm::mat4 Transform::getLocalMatrix() {
    if (isDirty) {
        recomputeLocal();
        isDirty = false;
    }
    return localMatrix;
}

glm::mat4 Transform::getWorldMatrix() {
    if (parent) {
        return parent->getWorldMatrix() * getLocalMatrix();
    }
    return getLocalMatrix();
}

void Transform::lookAt(const glm::vec3& target, const glm::vec3& up) {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));
    glm::vec3 realUp  = glm::cross(right, forward);

    localMatrix = glm::mat4(
        glm::vec4(right,    0),
        glm::vec4(realUp,   0),
        glm::vec4(forward,  0),
        glm::vec4(position, 1)
    );
    localMatrix = glm::scale(localMatrix, scale);
    isDirty = false;
}

void Transform::recomputeLocal() {
    glm::mat4 m(1.0f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    m = glm::scale(m, scale);
    localMatrix = m;
}