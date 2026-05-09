#pragma once

#include "Mesh/Mesh.hpp"
#include "Transform.hpp"
#include "Material/Material.hpp"
#include "Camera.hpp"

#include <memory>
#include <vector>

struct SceneObject : std::enable_shared_from_this<SceneObject> {
    struct Builder {
        std::shared_ptr<SceneObject> object;

        Builder();

        Builder& withName(const std::string& name);
        Builder& withMesh(const std::shared_ptr<Mesh>& mesh);
        Builder& withMaterial(const std::shared_ptr<Material>& material);
        Builder& withPosition(const glm::vec3& position);
        Builder& withRotation(const glm::vec3& rotationDegrees);
        Builder& withScale(const glm::vec3& scale);
        Builder& withActive(bool active);
        Builder& withChild(const std::shared_ptr<SceneObject>& child);

        std::shared_ptr<SceneObject> build() const;
    };

    std::string name = "SceneObject";
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    std::weak_ptr<SceneObject> parent;
    std::vector<std::shared_ptr<SceneObject>> children;

    bool isActive = true;

    static Builder builder();

    void addChild(const std::shared_ptr<SceneObject>& child);
    virtual void update(float dt) { };
};