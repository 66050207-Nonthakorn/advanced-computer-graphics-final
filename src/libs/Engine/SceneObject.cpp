#include "Engine/SceneObject.hpp"

void SceneObject::addChild(const std::shared_ptr<SceneObject>& child) {
    child->parent = shared_from_this();
    child->transform.setParent(&transform);
    children.push_back(child);
}
