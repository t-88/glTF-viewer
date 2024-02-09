#pragma once

// jsoncpp
#include "json.h"
// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Mesh {
public:
    uint32_t vao;
    uint32_t vbo; 
    uint32_t ebo; 

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> buffer;
    std::vector<uint16_t> indices;
    
    Mesh(std::vector<float> _vertices,std::vector<uint16_t> _indices);
    Mesh(std::vector<float> _vertices,std::vector<float> _normals, std::vector<uint16_t> _indices,std::vector<std::vector<float>> textures);
    ~Mesh();
    void render();

    

};
