#include "Engine/Shader/Shader.hpp"
#include "Engine/Utils/FileUtils.hpp"

#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

Shader::Shader(
    const std::string &vertexPath,
    const std::string &fragmentPath
) {
    std::string vertexSrc = FileUtils::readFile(vertexPath);
    std::string fragmentSrc = FileUtils::readFile(fragmentPath);

    unsigned int vertex = compile(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragment = compile(GL_FRAGMENT_SHADER, fragmentSrc);

    this->program = glCreateProgram();
    glAttachShader(this->program, vertex);
    glAttachShader(this->program, fragment);
    glLinkProgram(this->program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const {
    glUseProgram(this->program);
}

void Shader::uniformFloat(const std::string& name, float value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniform1f(location, value);
}

void Shader::uniformInt(const std::string& name, int value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniform1i(location, value);
}

void Shader::uniformBool(const std::string& name, bool value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniform1i(location, static_cast<int>(value));
}

void Shader::uniformVec2(const std::string& name, const glm::vec2& value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::uniformVec3(const std::string& name, const glm::vec3& value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::uniformVec4(const std::string& name, const glm::vec4& value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::uniformMat4(const std::string& name, const glm::mat4& value) {
    int location = glGetUniformLocation(this->program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::bindTexture(int slot, unsigned int id, int target) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, id);
}

unsigned int Shader::compile(GLenum type, const std::string &source) {
    const char* sourceCStr = source.c_str();
    
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCStr, NULL);
    glCompileShader(shader);

    // print compile errors if any
    int isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    
    if (!isCompiled) {
        char messages[512];
        glGetShaderInfoLog(shader, 512, NULL, messages);
        
        std::cerr << "[Shader] compilation error " << messages << std::endl;
        exit(1);
    };

    return shader;
}
