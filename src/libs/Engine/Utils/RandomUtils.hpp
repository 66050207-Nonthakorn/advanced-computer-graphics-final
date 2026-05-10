#pragma once

#include <cstdlib>

class RandomUtils {
public:
    static float randRange(float low, float high) {
        return low + (high - low) * (static_cast<float>(std::rand()) / RAND_MAX);
    }
};