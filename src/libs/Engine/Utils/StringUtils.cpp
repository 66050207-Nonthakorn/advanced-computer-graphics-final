#include "Engine/Utils/StringUtils.hpp"

const std::string cubemapNameDirX[] {"x", "y", "z"};
const std::string cubemapNameDirY[] {"p", "n"};

std::array<std::string, 6> StringUtils::getHDRCubemapFileNames(const std::string& dir) {
    std::array<std::string, 6> names;

    int i = 0;
    for (const auto &x: cubemapNameDirX) {
        for (const auto &y: cubemapNameDirY) {
            names[i++] = (dir + "/" + y + x + ".hdr");
        }
    }

    return names;
}

std::vector<std::array<std::string, 6>> StringUtils::getHDRCubemapMipFileNames(
    const std::string& dir,
    const int total
) {
    std::vector<std::array<std::string, 6>> mipNames;
    if (total <= 0) {
        return mipNames;
    }

    mipNames.reserve(total);
    
    for (int mip = 0; mip < total; ++mip) {
        std::array<std::string, 6> names;

        int i = 0;
        for (const auto &x: cubemapNameDirX) {
            for (const auto &y: cubemapNameDirY) {
                names[i++] = (dir + "/m" + std::to_string(mip) + "_" + y + x + ".hdr");
            }
        }

        mipNames.emplace_back(names);
    }

    return mipNames;
}