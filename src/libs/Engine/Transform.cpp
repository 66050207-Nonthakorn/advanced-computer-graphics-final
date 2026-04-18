#include "Engine/Transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

Transform::Transform()
    : isDirty(true), position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), model(1.0f) {}

glm::mat4 Transform::getModel() {
    if (!isDirty) {
        return this->model;
    }
 
    isDirty = false;

    glm::mat4 model(1.0f);
    
    model = glm::translate(model, this->position);
    model = glm::rotate(model, glm::radians(this->rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(this->rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(this->rotation.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, this->scale);

    return this->model = model;
}

glm::vec3 Transform::getPosition() const {
    return this->position;
}

glm::vec3 Transform::getRotation() const {
    return this->rotation;
}

glm::vec3 Transform::getScale() const {
    return this->scale;
}

void Transform::setPosition(const glm::vec3 &position) {
    isDirty = true;
    this->position = position;
}

void Transform::setRotation(const glm::vec3 &rotation) {
    isDirty = true;
    this->rotation = rotation;
}

void Transform::setScale(const glm::vec3 &scale) {
    isDirty = true;
    this->scale = scale;
}
