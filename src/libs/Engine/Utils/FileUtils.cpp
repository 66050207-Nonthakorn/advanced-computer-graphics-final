#include "Engine/Utils/FileUtils.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

std::string FileUtils::readFile(const std::string& path) {
    std::ifstream file(path);
    
    if (!file.is_open()) {
        std::cerr << "[FileUtils] cannot open file: " + path << std::endl;
        exit(1);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}