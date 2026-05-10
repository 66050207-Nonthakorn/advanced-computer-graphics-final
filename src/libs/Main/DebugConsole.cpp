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
        "  wind <float>                         set cloth wind gust (default: 15)\n"
        "  light [on|off]                       show/hide all light specular highlights (default: show)\n"
        "  ibl [on|off]                         toggle image-based lighting (default: on)\n"
        "  pointlight [icon|noicon]             show/hide point-light debug icons (default: noicon)\n"
        "  pointlight [animate|noanimate]       toggle pulsing animation on first point light (default: noanimate)\n"
        "  particle [on|off]                    show/hide particle system (default: on)\n"
        "  camera [lock|unlock]                 lock/unlock camera (default: lock)\n"
        "  camera reset                         reset camera to default position\n"
        "  draw [full|line|point]               set draw mode (default: full)\n"
        "  shading [norm|shadow|amb|diff|spec]  set shading mode (default: norm)\n"
        "  help                                 show this message\n"
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

    Command cmd;
    ss >> cmd.cmd;

    auto readFloat = [&](float& v) {
        return bool(ss >> v);
    };
    auto readInt = [&](int& v) {
        return bool(ss >> v);
    };
    auto readString = [&](std::string& v) {
        return bool(ss >> v);
    };
    auto readVec3  = [&](float& x, float& y, float& z) {
        return bool(ss >> x >> y >> z);
    };

    if (cmd.cmd == "help") {
        printHelp();
        return;
    }
    else if (cmd.cmd == "wind") {
        if (!readFloat(cmd.f0)) goto bad;
        if (cmd.f0 < 0) cmd.f0 = 0;
    }
    else if (cmd.cmd == "light") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "on" &&
            cmd.arg != "off") goto bad;
    }
    else if (cmd.cmd == "ibl") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "on" &&
            cmd.arg != "off") goto bad;
    }
    else if (cmd.cmd == "pointlight") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "icon" &&
            cmd.arg != "noicon" &&
            cmd.arg != "animate" &&
            cmd.arg != "noanimate") goto bad;
    }
    else if (cmd.cmd == "particle") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "on" &&
            cmd.arg != "off") goto bad;
    }
    else if (cmd.cmd == "camera") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "lock" &&
            cmd.arg != "unlock" &&
            cmd.arg != "reset") goto bad;
    }
    else if (cmd.cmd == "draw") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "full" &&
            cmd.arg != "line" &&
            cmd.arg != "point") goto bad;
    }
    else if (cmd.cmd == "shading") {
        if (!readString(cmd.arg)) goto bad;
        if (cmd.arg != "norm" &&
            cmd.arg != "shadow" &&
            cmd.arg != "amb" &&
            cmd.arg != "diff" &&
            cmd.arg != "spec") goto bad;
    }
    else {
        std::cout << "[Debug] Unknown command: '" << cmd.cmd << "'. Type 'help'.\n";
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push_back(cmd);
    }
    
    return;

bad:
    std::cout << "[Debug] Bad command arguments for '" << cmd.cmd << "'. Type 'help'.\n";
}
