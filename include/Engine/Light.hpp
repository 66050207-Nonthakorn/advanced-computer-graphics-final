#pragma once

#include "glm/vec3.hpp"

struct Light {
    glm::vec3 position;
    float ambient;
    float diffuse;
    float specular;
};