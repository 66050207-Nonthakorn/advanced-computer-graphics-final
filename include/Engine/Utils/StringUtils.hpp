#pragma once

#include <array>
#include <vector>
#include <string>

class StringUtils {
public:
    static std::array<std::string, 6>
        getHDRCubemapFileNames(const std::string& dir); 
    
    static std::vector<std::array<std::string, 6>>
        getHDRCubemapMipFileNames(const std::string& dir, const int total); 
};