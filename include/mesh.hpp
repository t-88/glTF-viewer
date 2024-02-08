#pragma once

#include "pch.hpp"
class Mesh {
public:
    uint32_t vao;
    uint32_t vbo; 
    uint32_t ebo; 

    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    
    Mesh(std::vector<float> _vertices,std::vector<uint16_t> _indices);
    ~Mesh();
    void render();

    

};
