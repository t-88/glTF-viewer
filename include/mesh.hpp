#pragma once


// jsoncpp
#include "json.h"
// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gltf_loader.hpp"
#include "shader.hpp"

#include "pch.hpp"


#define BASE_TEXTURE_KEY "base-texture"
#define NORMAL_TEXTURE_KEY "normal-texture"

class Mesh {
public:
    uint32_t vao;
    uint32_t vbo; 
    uint32_t ebo; 

    std::vector<float> buffer;
    std::vector<uint16_t> indices;
    int vertices_count = 0;

    Shader shader;

    std::map<std::string,uint32_t> texture_idxs;
    uint32_t texture0;

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

    void setup_textures(std::string key,GltfTextureData texture_data);
    void setup_textures(std::map<std::string,GltfTextureData> texture_data);
    void render();

};
