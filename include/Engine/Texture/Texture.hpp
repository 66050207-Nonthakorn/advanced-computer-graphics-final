#pragma once

#include <string>

struct Texture {    
    unsigned int id;
    int width, height, nrChannels;

    Texture(const std::string& filePath, const int format, const bool isFlip);
    Texture(unsigned char* data, int width, int height, int nrChannels, const int format);
    ~Texture();
};