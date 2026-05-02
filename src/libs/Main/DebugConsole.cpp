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
        "  light.pos <x> <y> <z>          set point light[0] position\n"
        "  light.intensity <val>          set point light[0] intensity\n"
        "  light.color <r> <g> <b>        set point light[0] color\n"
        "  dir.dir <x> <y> <z>            set directional light direction\n"
        "  dir.intensity <val>            set directional light intensity\n"
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
    else if (token == "light.pos") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::LightPos;
    }
    else if (token == "light.intensity") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::LightIntensity;
    }
    else if (token == "light.color") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::LightColor;
    }
    else if (token == "dir.dir") {
        if (!readVec3(cmd.f0, cmd.f1, cmd.f2)) goto bad;
        cmd.type = Command::Type::DirLightDir;
    }
    else if (token == "dir.intensity") {
        if (!readFloat(cmd.f0)) goto bad;
        cmd.type = Command::Type::DirLightIntensity;
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
