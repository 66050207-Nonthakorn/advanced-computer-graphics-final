#pragma once

#include <string>

struct Texture {    
    unsigned int id;
    int width, height, nrChannels;

    Texture(const std::string& filePath);
};