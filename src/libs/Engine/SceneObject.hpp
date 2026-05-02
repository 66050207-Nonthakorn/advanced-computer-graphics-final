#pragma once

#include "Mesh/Mesh.hpp"
#include "Transform.hpp"
#include "Material/Material.hpp"
#include "Camera.hpp"

#include <memory>

struct SceneObject {
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
};