#pragma once

#include <string>
#include "glm/vec3.hpp"

struct Constants {
    static inline constexpr int SCREEN_WIDTH = 800;
    static inline constexpr int SCREEN_HEIGHT = 600;
    static inline constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    static inline const std::string SCREEN_TITLE = "Advanced Computer Graphics Final";

    static inline const glm::vec3 CAMERA_START_POS = glm::vec3(-0.239992, 5.96176, 6.7077);
    static inline const glm::vec3 CAMERA_START_TARGET = glm::vec3(0.462143, 5.84335, 6.07629);

    static inline constexpr float DIRECTIONAL_INTENSITY = 15.0f;
    static inline constexpr float POINTLIGHT_INTENSITY = 50.0f;
    static inline constexpr float PARTICLE_EMIT_RATE = 150.0f;
    static inline constexpr float IBL_INTENSITY = 0.075f;
};