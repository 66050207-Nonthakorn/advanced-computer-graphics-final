#include "Engine/Mesh/MeshLoader.hpp"
#include "Engine/Utils/FileUtils.hpp"

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

MeshData MeshLoader::loadFromObj(const std::string &path) {
    std::cout << "[MeshLoader] loading " << path << std::endl;

    float t = glfwGetTime();

    std::string content;
    try {
        content = FileUtils::readFile(path);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    std::istringstream fileStream(content);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    MeshData result;
    std::unordered_map<std::string, unsigned int> indexCache;
    std::vector<unsigned int> faceIndices;

    auto getIndex = [&](const std::string &token) -> unsigned int {
        // Reserve slot in the map; skip parsing entirely on cache hit
        auto [it, inserted] = indexCache.try_emplace(token, (unsigned int)result.vertices.size());
        if (!inserted)
            return it->second;

        // token format: v[/vt[/vn]] — parse with strtol to avoid istringstream overhead
        int vi = 0, ti = 0, ni = 0;
        const char* p = token.c_str();
        char* end;
        vi = (int)std::strtol(p, &end, 10);
        if (*end == '/') {
            p = end + 1;
            if (*p != '/') { ti = (int)std::strtol(p, &end, 10); p = end; }
            if (*p == '/')  { ni = (int)std::strtol(p + 1, nullptr, 10); }
        }

        Vertex v{};
        if (vi != 0) {
            int idx = vi > 0 ? vi - 1 : (int)positions.size() + vi;
            if (idx >= 0 && idx < (int)positions.size()) v.position = positions[idx];
        }
        if (ni != 0) {
            int idx = ni > 0 ? ni - 1 : (int)normals.size() + ni;
            if (idx >= 0 && idx < (int)normals.size()) v.normal = normals[idx];
        }
        if (ti != 0) {
            int idx = ti > 0 ? ti - 1 : (int)uvs.size() + ti;
            if (idx >= 0 && idx < (int)uvs.size()) v.uv = uvs[idx];
        }

        result.vertices.emplace_back(v);
        return it->second;
    };

    std::string line;
    while (std::getline(fileStream, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string keyword;
        ss >> keyword;

        if (keyword == "v") {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.emplace_back(p);
        }
        else if (keyword == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.emplace_back(n);
        }
        else if (keyword == "vt") {
            glm::vec2 t;
            ss >> t.x >> t.y;
            uvs.emplace_back(t);
        }
        else if (keyword == "f") {
            // Fan triangulation for polygons with 3+ vertices
            faceIndices.clear();
            std::string token;
            while (ss >> token)
                faceIndices.emplace_back(getIndex(token));

            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                result.indices.emplace_back(faceIndices[0]);
                result.indices.emplace_back(faceIndices[i]);
                result.indices.emplace_back(faceIndices[i + 1]);
            }
        }
    }

    // Generate flat normals for OBJ exported without vn
    bool hasNormals = false;
    for (auto& v : result.vertices) {
        if (glm::dot(v.normal, v.normal) > 0.0f) {
            hasNormals = true;
            break;
        }
    }

    if (!hasNormals) {
        for (size_t i = 0; i + 2 < result.indices.size(); i += 3) {
            auto& v0 = result.vertices[result.indices[i]];
            auto& v1 = result.vertices[result.indices[i + 1]];
            auto& v2 = result.vertices[result.indices[i + 2]];
            glm::vec3 n = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
            v0.normal += n;
            v1.normal += n;
            v2.normal += n;
        }

        for (auto& v : result.vertices) {
            if (glm::dot(v.normal, v.normal) > 0.0f) {
                v.normal = glm::normalize(v.normal);
            }
        }
    }

    std::cout << "[MeshLoader] took " << glfwGetTime() - t << std::endl;

    return result;
}