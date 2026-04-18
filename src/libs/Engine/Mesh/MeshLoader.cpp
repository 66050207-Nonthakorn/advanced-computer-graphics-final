#include "Engine/Mesh/MeshLoader.hpp"
#include "Engine/Utils/FileUtils.hpp"

#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

MeshData MeshLoader::loadFromObj(const std::string &path) {
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

    auto getIndex = [&](const std::string &token) -> unsigned int {
        auto it = indexCache.find(token);
        if (it != indexCache.end())
            return it->second;

        // token format: v[/vt[/vn]]
        int vi = 0, ti = 0, ni = 0;
        char sep;
        std::istringstream ss(token);

        // Parse position index (required)
        ss >> vi;
        if (ss.peek() == '/') {
            ss >> sep;
            if (ss.peek() != '/') ss >> ti;
            if (ss.peek() == '/') { ss >> sep; ss >> ni; }
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

        unsigned int newIndex = static_cast<unsigned int>(result.vertices.size());
        result.vertices.push_back(v);
        indexCache[token] = newIndex;
        return newIndex;
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
            positions.push_back(p);
        }
        else if (keyword == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (keyword == "vt") {
            glm::vec2 t;
            ss >> t.x >> t.y;
            uvs.push_back(t);
        }
        else if (keyword == "f") {
            // Fan triangulation for polygons with 3+ vertices
            std::vector<unsigned int> faceIndices;
            std::string token;
            while (ss >> token)
                faceIndices.push_back(getIndex(token));

            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                result.indices.push_back(faceIndices[0]);
                result.indices.push_back(faceIndices[i]);
                result.indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    return result;
}