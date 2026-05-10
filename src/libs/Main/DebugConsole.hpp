#pragma once

#include "Engine/Cloth/ClothMesh.hpp"
#include "Engine/Light/PointLight.hpp"
#include "Engine/Light/DirectionalLight.hpp"
#include "Engine/SceneObject.hpp"

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <memory>

// Runs on a background thread, reads lines from stdin, and queues mutations
// that MainScene::update applies on the main thread each frame.
class DebugConsole {
public:
    struct Command {
        int index = 0;
        std::string cmd, arg;
        float f0 = 0, f1 = 0, f2 = 0;
    };

    DebugConsole();
    ~DebugConsole();

    void poll(std::vector<Command>& out);

private:
    void run();
    void parse(const std::string& line);

    std::thread thread;
    std::atomic<bool> isQuit{ false };
    std::mutex mutex;
    std::vector<Command> queue;
};
