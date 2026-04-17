#pragma once

#include <vector>

class mesh {
public:
    mesh();
    ~mesh();

    void draw();
private:
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
