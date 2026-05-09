#pragma once

#include <cstdlib>

class RandomUtils {
public:
    // Returns a random float in the range [low, high].
    static float randRange(float low, float high) {
        return low + (high - low) * (static_cast<float>(std::rand()) / RAND_MAX);
    }
};