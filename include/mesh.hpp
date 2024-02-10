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
    uint32_t texture0 = -1;


    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> buffer;
    std::vector<uint16_t> indices;

    Shader shader;

    // transformations
    glm::vec3 rotation;
    glm::mat4x4 rotation_mat;
    glm::vec3 scale;
    glm::mat4x4 scale_mat;
    glm::vec3 translate;
    glm::mat4x4 translation_mat;

    
    Mesh(GltfLoader gltf_loader);
    Mesh(std::vector<float> _vertices,std::vector<uint16_t> _indices);
    ~Mesh();
    void render();

    

    void setup_vertices(std::vector<float> _vertices,std::vector<float> _normals, std::vector<uint16_t> _indices); 
    void setup_vertices(std::vector<float> _vertices,std::vector<float> _normals,std::vector<float> texture_data, std::vector<uint16_t> _indices);

    void setup_textures(std::map<std::string,GltfTextureData> texture_data);
    void setup_transformations();
    void setup_shader(GltfLoader gltf_loader);

};
