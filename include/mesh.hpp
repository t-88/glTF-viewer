#pragma once


// jsoncpp
#include "json.h"
// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gltf_loader.hpp"
#include "shader.hpp"

#include "pch.hpp"


class Mesh {
public:
    uint32_t vao;
    uint32_t vbo; 
    uint32_t ebo; 

    std::vector<float> buffer;
    std::vector<uint16_t> indices;
    int vertices_count = 0;

    Shader shader;

    // transformations
    glm::vec3 rotation;
    glm::mat4x4 rotation_mat;
    glm::vec3 scale;
    glm::mat4x4 scale_mat;
    glm::vec3 translate;
    glm::mat4x4 translation_mat;

    
    Mesh(GltfLoader gltf_loader);
    ~Mesh();
    
    void setup_transformations();
    void setup_shader(GltfLoader gltf_loader);
    void setup_vertices(GltfLoader gltf_loader);
    void setup_textures(std::map<std::string,GltfTextureData> texture_data);
    void render();

};
