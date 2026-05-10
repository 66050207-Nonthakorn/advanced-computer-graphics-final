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

}

void Renderer::draw(Scene& scene) {
    // 1. Flatten and categorize nodes
    std::vector<FlatNode> opaqueNodes;
    std::vector<FlatNode> transparentNodes;
    std::vector<FlatNode> overlayNodes;

    for (auto& root : scene.sceneObjects) {
        std::vector<FlatNode> flat;
        collectNodes(root, flat);
        for (auto& node : flat) {
            if (node.obj->material->isOverlay()) {
                overlayNodes.push_back(node);
            } else if (node.obj->material->isTransparent()) {
                transparentNodes.push_back(node);
            } else {
                opaqueNodes.push_back(node);
            }
        }
    }

    // Shadow pass
    glm::mat4 lightSpaceMat = scene.directionalLight.getLightSpaceMatrix();
    auto shadowShader = ShaderManager::instance().get("shadow");

    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    glBindFramebuffer(GL_FRAMEBUFFER, scene.directionalLight.getShadowFBO());
    glClear(GL_DEPTH_BUFFER_BIT);

    GLboolean cullWasEnabled = glIsEnabled(GL_CULL_FACE);
    GLint previousCullFaceMode = GL_BACK;
    glGetIntegerv(GL_CULL_FACE_MODE, &previousCullFaceMode);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    shadowShader->use();
    shadowShader->uniformMat4("lightSpaceMatrix", lightSpaceMat);

    for (auto& node : opaqueNodes) {
        if (dynamic_cast<ParticleMaterial*>(node.obj->material.get())) continue;
        
        shadowShader->uniformMat4("model", node.worldMatrix);
        glBindVertexArray(node.obj->mesh->vao);
        glDrawElements(node.obj->mesh->drawMode, node.obj->mesh->indexCount, GL_UNSIGNED_INT, 0);
    }

    glCullFace(previousCullFaceMode);
    if (!cullWasEnabled) {
        glDisable(GL_CULL_FACE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glViewport(0, 0, window.width, window.height);

    glm::vec3 camPos = scene.camera->position;

    // Sort Opaque By Material
    std::sort(opaqueNodes.begin(), opaqueNodes.end(), [](const auto& a, const auto& b) {
        return a.obj->material.get() < b.obj->material.get();
    });

    // Sort TransparentBack-to-Front
    std::sort(transparentNodes.begin(), transparentNodes.end(), [&camPos](const auto& a, const auto& b) {
        glm::vec3 deltaA = glm::vec3(a.worldMatrix[3]) - camPos;
        glm::vec3 deltaB = glm::vec3(b.worldMatrix[3]) - camPos;
        float distA = glm::dot(deltaA, deltaA);
        float distB = glm::dot(deltaB, deltaB);
        return distA > distB;
    });

    // Main render pass
    auto drawList = [&](std::vector<FlatNode>& nodes) {
        Material* lastMaterial = nullptr;
        for (auto& node : nodes) {
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
            mat->bindPerObject({ node.worldMatrix });
            glBindVertexArray(node.obj->mesh->vao);
            glDrawElements(node.obj->mesh->drawMode, node.obj->mesh->indexCount, GL_UNSIGNED_INT, 0);
        }
    };

    // Execute Passes in order
    drawList(opaqueNodes);
    
    // Draw Cubemap (handled depth settings internally)
    auto drawCubemap = [&]() {
        if (scene.cubemap.environmentMap == nullptr) return;

        GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
        if (cullEnabled) {
            glDisable(GL_CULL_FACE);
        }

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

        if (cullEnabled) {
            glEnable(GL_CULL_FACE);
        }
    };
    drawCubemap();

    // Transparent Pass
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    drawList(transparentNodes);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Overlay Pass
    glDisable(GL_DEPTH_TEST);
    drawList(overlayNodes);
    glEnable(GL_DEPTH_TEST);
}