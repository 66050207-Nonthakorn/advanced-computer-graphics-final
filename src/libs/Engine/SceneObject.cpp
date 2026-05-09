#include "Engine/SceneObject.hpp"
#include "SceneObject.hpp"

SceneObject::Builder::Builder()
    : object(std::make_shared<SceneObject>()) {
}

SceneObject::Builder& SceneObject::Builder::withName(const std::string &name) {
    object->name = name;
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withMesh(const std::shared_ptr<Mesh>& mesh) {
    object->mesh = mesh;
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withMaterial(const std::shared_ptr<Material>& material) {
    object->material = material;
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withPosition(const glm::vec3& position) {
    object->transform.setPosition(position);
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withRotation(const glm::vec3& rotationDegrees) {
    object->transform.setRotation(rotationDegrees);
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withScale(const glm::vec3& scale) {
    object->transform.setScale(scale);
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withActive(bool active) {
    object->isActive = active;
    return *this;
}

SceneObject::Builder& SceneObject::Builder::withChild(const std::shared_ptr<SceneObject>& child) {
    object->addChild(child);
    return *this;
}

std::shared_ptr<SceneObject> SceneObject::Builder::build() const {
    return object;
}

SceneObject::Builder SceneObject::builder() {
    return Builder();
}

void SceneObject::addChild(const std::shared_ptr<SceneObject>& child) {
    child->parent = shared_from_this();
    child->transform.setParent(&transform);
    children.push_back(child);
}
