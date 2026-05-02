#pragma once

#include "Engine/Vertex.hpp"
#include "Engine/Mesh/Mesh.hpp"

#include "glm/glm.hpp"

#include <vector>
#include <memory>

struct ClothParticle {
    glm::vec3 position;
    glm::vec3 prevPosition;
    glm::vec3 normal;
    glm::vec2 uv;
    bool pinned = false;
};

struct ClothSpring {
    int a, b;
    float restLength;
};

class ClothMesh : public Mesh {
public:
    ClothMesh(int rows, int cols, float width, float height);

    void update(float dt);

    void pin(int index);
    void unpin(int index);

    glm::vec3 gravity  = { 0.0f, -9.8f, 0.0f };
    glm::vec3 wind     = { 0.0f,  0.0f, 0.0f };
    float     windGustAmplitude = 0.0f;
    float     damping  = 0.99f;
    int       solverIterations = 10;

private:
    int rows, cols;
    std::vector<ClothParticle> particles;
    std::vector<ClothSpring> springs;
    std::vector<Vertex> cpuVertices;

    void buildGrid(float width, float height);
    void addSpring(int a, int b);
    void applyWind(float dt);
    void solveConstraints();
    void recalcNormals();
    void uploadToGPU();
};
