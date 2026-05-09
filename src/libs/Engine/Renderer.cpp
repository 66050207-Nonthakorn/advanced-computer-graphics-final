#include "Engine/Renderer.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Manager/ShaderManager.hpp"
#include "Engine/Material/ParticleMaterial.hpp"

#include "GL/glew.h"
#include "Engine/Light/DirectionalLight.hpp"
#include <algorithm>
#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"
#include "Renderer.hpp"

Renderer::Renderer(const Window &window)
    : window(window)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::clearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

namespace {

struct FlatNode {
    SceneObject* obj;
    glm::mat4 worldMatrix;
};

void collectNodes(const std::shared_ptr<SceneObject>& node, std::vector<FlatNode>& out) {
    if (node->mesh && node->isActive) {
        out.push_back({ node.get(), node->transform.getWorldMatrix() });
    }

    for (auto& child : node->children) {
        collectNodes(child, out);
    }
}

} // namespace

void Renderer::draw(Scene& scene) {
    // Flatten scene hierarchy into renderable nodes
    std::vector<FlatNode> allNodes;
    for (auto& root : scene.sceneObjects) {
        collectNodes(root, allNodes);
    }

    // Shadow pass
    glm::mat4 lightSpaceMat = scene.directionalLight.getLightSpaceMatrix();
    auto shadowShader = ShaderManager::instance().get("shadow");

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, scene.directionalLight.getShadowFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    shadowShader->use();
    shadowShader->uniformMat4("lightSpaceMatrix", lightSpaceMat);

    for (auto& node : allNodes) {
        if (dynamic_cast<ParticleMaterial*>(node.obj->material.get())) continue;
        if (node.obj->material->isOverlay()) continue;
        shadowShader->uniformMat4("model", node.worldMatrix);
        glBindVertexArray(node.obj->mesh->vao);
        glDrawElements(GL_TRIANGLES, node.obj->mesh->indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Lighting pass — opaque first (grouped by material), transparent last (back-to-front)
    glm::vec3 camPos = scene.camera->position;
    std::sort(allNodes.begin(), allNodes.end(),
        [&camPos](const auto& a, const auto& b) {
            if (a.obj->material.get() != b.obj->material.get())
                return a.obj->material.get() < b.obj->material.get(); // group opaques
            
            // far to near
            glm::vec3 pa = glm::vec3(a.worldMatrix[3]);
            glm::vec3 pb = glm::vec3(b.worldMatrix[3]);
            
            return glm::dot(pa - camPos, pa - camPos) > glm::dot(pb - camPos, pb - camPos);
        });

    auto drawNodes = [&](bool transparent) {
        Material* lastMaterial = nullptr;
        for (auto& node : allNodes) {
            if (node.obj->material->isOverlay()) continue;
            if (node.obj->material->isTransparent() != transparent) continue;
            Material* mat = node.obj->material.get();
            if (mat != lastMaterial) {
                unsigned int iblIrr  = scene.cubemap.iblIrradianceMap ? scene.cubemap.iblIrradianceMap->getId() : 0;
                unsigned int iblPref = scene.cubemap.iblPrefilterMap  ? scene.cubemap.iblPrefilterMap->getId()  : 0;
                unsigned int iblLUT  = scene.cubemap.iblBrdfLUT       ? scene.cubemap.iblBrdfLUT->getId()       : 0;
                float iblIntensity = scene.cubemap.iblIntensity;
                int iblMips = scene.cubemap.iblPrefilterMips;

                mat->bindPerFrame({ *scene.camera, scene.lights, scene.directionalLight,
                                    scene.directionalLight.getDepthMapID(), lightSpaceMat,
                                    iblIrr, iblPref, iblMips, iblLUT, iblIntensity });
                lastMaterial = mat;
            }
            node.obj->material->bindPerObject({ node.worldMatrix });
            glBindVertexArray(node.obj->mesh->vao);
            glDrawElements(GL_TRIANGLES, node.obj->mesh->indexCount, GL_UNSIGNED_INT, 0);
        }
    };

    auto drawCubemap = [&]() {
        if (scene.cubemap.environmentMap == nullptr) return;

        auto cubemapShader = ShaderManager::instance().get("cubemap");
        cubemapShader->use();
        cubemapShader->uniformMat4("view", scene.camera->getView());
        cubemapShader->uniformMat4("projection", scene.camera->getProjection());
        cubemapShader->uniformInt("cubemap", 0);

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        scene.cubemap.environmentMap->bind(0);

        glBindVertexArray(scene.cubemap.cube->vao);
        glDrawElements(GL_TRIANGLES, scene.cubemap.cube->indexCount, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    };

    drawNodes(false); // Draw alpha = 1.0
    drawCubemap();

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    drawNodes(true); // Draw alpha < 1.0
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    // Overlay pass — billboards always render on top
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    {
        Material* lastMaterial = nullptr;
        for (auto& node : allNodes) {
            if (!node.obj->material->isOverlay()) continue;
            Material* mat = node.obj->material.get();
            if (mat != lastMaterial) {
                unsigned int iblIrr  = scene.cubemap.iblIrradianceMap ? scene.cubemap.iblIrradianceMap->getId() : 0;
                unsigned int iblPref = scene.cubemap.iblPrefilterMap  ? scene.cubemap.iblPrefilterMap->getId()  : 0;
                unsigned int iblLUT  = scene.cubemap.iblBrdfLUT       ? scene.cubemap.iblBrdfLUT->getId()       : 0;
                mat->bindPerFrame({ *scene.camera, scene.lights, scene.directionalLight,
                                    scene.directionalLight.getDepthMapID(), lightSpaceMat,
                                    iblIrr, iblPref, scene.cubemap.iblPrefilterMips, iblLUT, scene.cubemap.iblIntensity });
                lastMaterial = mat;
            }
            node.obj->material->bindPerObject({ node.worldMatrix });
            glBindVertexArray(node.obj->mesh->vao);
            glDrawElements(GL_TRIANGLES, node.obj->mesh->indexCount, GL_UNSIGNED_INT, 0);
        }
    }
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
