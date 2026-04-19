#include "Engine/SceneObject.hpp"
#include "glm/glm.hpp"

void SceneObject::draw(const Camera& camera) {
    // Bind Material
    this->material->shader->use();
    this->material->shader->uniformMat4("model", this->transform.getModel());
    this->material->shader->uniformMat4("view", camera.getView());
    this->material->shader->uniformMat4("projection", camera.getProjection());

    this->material->shader->uniformVec3("viewPosition", camera.position);
    this->material->shader->uniformVec3("objectColor", glm::vec3(1, 0, 0));

    // Draw Mesh
    glBindVertexArray(this->mesh->vao);
    glDrawElements(GL_TRIANGLES, this->mesh->indexCount, GL_UNSIGNED_INT, 0);
}