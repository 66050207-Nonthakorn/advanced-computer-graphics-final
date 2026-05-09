#pragma once

#include <string>

struct Constants {
    static inline constexpr int SCREEN_WIDTH = 800;
    static inline constexpr int SCREEN_HEIGHT = 600;
    static inline constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    static inline const std::string SCREEN_TITLE = "Advanced Computer Graphics Final";
};