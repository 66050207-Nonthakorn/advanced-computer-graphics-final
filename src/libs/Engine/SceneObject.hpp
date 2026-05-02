#pragma once

#include "Mesh/Mesh.hpp"
#include "Transform.hpp"
#include "Material/Material.hpp"
#include "Camera.hpp"

#include <memory>
#include <vector>

struct SceneObject : std::enable_shared_from_this<SceneObject> {
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    std::weak_ptr<SceneObject> parent;
    std::vector<std::shared_ptr<SceneObject>> children;

    void addChild(const std::shared_ptr<SceneObject>& child);
};