#pragma once

#include <string>
#include "GL/glew.h"
#include "glm/glm.hpp"

class Shader {
public:
    Shader(
        const std::string& vertexPath,
        const std::string& fragmentPath 
    );

    void use() const;

    void uniformFloat(const std::string &name, float value);
    void uniformInt(const std::string &name, int value);
    void uniformBool(const std::string &name, bool value);
    void uniformVec2(const std::string &name, const glm::vec2 &value);
    void uniformVec3(const std::string &name, const glm::vec3 &value);
    void uniformVec4(const std::string &name, const glm::vec4 &value);
    void uniformMat4(const std::string &name, const glm::mat4 &value);

private:
    unsigned int program;
    unsigned int compile(GLenum type, const std::string& source);
};