#include "Main/DebugConsole.hpp"

#include <iostream>
#include <sstream>

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

DebugConsole::DebugConsole() {
    thread = std::thread(&DebugConsole::run, this);
}

DebugConsole::~DebugConsole() {
    isQuit.store(true);
    thread.detach();
}

void DebugConsole::poll(std::vector<Command>& out) {
    std::lock_guard<std::mutex> lock(mutex);
    out.insert(out.end(), queue.begin(), queue.end());
    queue.clear();
}

static void printHelp() {
    std::cout <<
        "\n--- Debug Console Commands ---\n"
        "  cloth.wind <x> <y> <z>         set cloth wind vector\n"
        "  cloth.windGust <amp>           set wind gust amplitude\n"
        "  cloth.damping <val>            set cloth damping (0-1)\n"
        "  cloth.gravity <x> <y> <z>      set gravity vector\n"
        "  cloth.iterations <n>           set solver iterations\n"
    "  pointlight.show <0|1>          show/hide point-light debug icons\n"
    "  pointlight.intensity <v>       set all point-light intensities\n"
    "  pointlight.color <r> <g> <b>   set all point-light colors\n"
    "  pointlight.animate <0|1>       toggle pulsing animation on first point light\n"
    "  dirlight.direction <x> <y> <z> set directional light direction\n"
    "  dirlight.intensity <v>         set directional light intensity\n"
    "  dirlight.color <r> <g> <b>     set directional light color\n"
    "  camera.lock [0|1]              lock/unlock camera (default: 1)\n"
    "  camera.unlock                  unlock camera\n"
        "  camera.reset                   reset camera to default position\n"
        "  draw.full                      set draw mode to normal\n"
        "  draw.line                      set draw mode to draw line only\n"
        "  draw.point                     set draw mode to draw point only\n"
    "  shading.normal                 normal shading\n"
    "  shading.shadow                 shadow-only debug shading\n"
    "  shading.ambient                ambient-only debug shading\n"
    "  shading.diffuse                diffuse-only debug shading\n"
    "  shading.specular               specular-only debug shading\n"
    "  shading.debug <0-4>            alias: 0=normal 1=shadow 2=ambient 3=diffuse 4=specular\n"
        "  help                           show this message\n"
        "------------------------------\n";
}

void DebugConsole::run() {
    std::cout << "[Debug] Console ready. Type 'help' for commands.\n> " << std::flush;

    std::string line;
    while (!isQuit.load() && std::getline(std::cin, line)) {
        if (!line.empty()) {
            parse(line);
        }
        if (!isQuit.load()) {
            std::cout << "> " << std::flush;
        }
    }
}

void DebugConsole::parse(const std::string& line) {
    std::istringstream ss(line);
    std::string token;
    ss >> token;

    auto readFloat = [&](float& v) {
        return bool(ss >> v);
    };
    auto readInt = [&](int& v) {
        return bool(ss >> v);
    };
    auto readVec3  = [&](float& x, float& y, float& z) {
        return bool(ss >> x >> y >> z);
    };

    Command cmd;

    if (token == "help") {
        printHelp();
        return;
    }
    else if (token == "cloth.wind") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::ClothWind;
    }
    else if (token == "cloth.windGust") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::ClothWindGust;
    }
    else if (token == "cloth.damping") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::ClothDamping;
    }
    else if (token == "cloth.gravity") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::ClothGravity;
    }
    else if (token == "cloth.iterations") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::ClothIterations;
    }
    else if (token == "pointlight.show") {
        if (!readFloat(cmd.f0) || (cmd.f0 < 0.0f || cmd.f0 > 1.0f)) goto bad;
        cmd.type = Command::Type::PointLightDebug;
    }
    else if (token == "pointlight.intensity") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::PointLightIntensity;
    }
    else if (token == "pointlight.color") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::PointLightColor;
    }
    else if (token == "pointlight.animate") {
        if (!readFloat(cmd.f0) || (cmd.f0 < 0.0f || cmd.f0 > 1.0f)) goto bad;
        cmd.type = Command::Type::PointLightAnimate;
    }
    else if (token == "dirlight.direction") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::DirectionalLightDirection;
    }
    else if (token == "dirlight.intensity") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::DirectionalLightIntensity;
    }
    else if (token == "dirlight.color") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::DirectionalLightColor;
    }
    else if (token == "camera.lock") {
        if (!(ss >> cmd.f0)) {
            cmd.f0 = 1.0f;
        }
        if (cmd.f0 < 0.0f || cmd.f0 > 1.0f) goto bad;
        cmd.type = Command::Type::CameraLock;
    }
    else if (token == "camera.unlock") {
        cmd.type = Command::Type::CameraLock;
        cmd.f0 = 0.0f;
    }
    else if (token == "camera.reset") {
        cmd.type = Command::Type::CameraReset;
    }
    else if (token == "draw.full") {
        cmd.type = Command::Type::DrawFull;
    }
    else if (token == "draw.line") {
        cmd.type = Command::Type::DrawLine;
    }
    else if (token == "draw.point") {
        cmd.type = Command::Type::DrawPoint;
    }
    else if (token == "shading.normal") {
        cmd.type = Command::Type::ShadingNormal;
    }
    else if (token == "shading.shadow") {
        cmd.type = Command::Type::ShadingShadow;
    }
    else if (token == "shading.ambient") {
        cmd.type = Command::Type::ShadingAmbient;
    }
    else if (token == "shading.diffuse") {
        cmd.type = Command::Type::ShadingDiffuse;
    }
    else if (token == "shading.specular") {
        cmd.type = Command::Type::ShadingSpecular;
    }
    else if (token == "shading.debug") {
        int mode = 0;
        if (!readInt(mode)) goto bad;
        switch (mode) {
            case 0: cmd.type = Command::Type::ShadingNormal; break;
            case 1: cmd.type = Command::Type::ShadingShadow; break;
            case 2: cmd.type = Command::Type::ShadingAmbient; break;
            case 3: cmd.type = Command::Type::ShadingDiffuse; break;
            case 4: cmd.type = Command::Type::ShadingSpecular; break;
            default: goto bad;
        }
    }
    else {
        std::cout << "[Debug] Unknown command '" << token << "'. Type 'help'.\n";
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push_back(cmd);
    }
    
    std::cout << "[Debug] OK\n";
    return;

bad:
    std::cout << "[Debug] Bad arguments for '" << token << "'. Type 'help'.\n";
}
