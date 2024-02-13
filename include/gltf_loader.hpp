#pragma once

#include <fstream>
#include "json.h"
#include "pch.hpp"


struct GltfMesh {
    std::string normal_idx , position_idx ,indices_idx, colors_idx;
    std::vector<std::string> texture_idx;

    GltfMesh() {
        normal_idx = "";
        position_idx = "";
        indices_idx = "";
    }
};
struct GltfNode {
    std::vector<std::string> children;
    std::string mesh_idx = "";   
    glm::mat4x4 matrix = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
};
struct GltfScene {
    std::map<std::string, std::string> nodes;
};

struct GltfAccessor {
    std::string buffer_view_idx = "";
    std::array<float,3> max = {1,1,1};
    std::array<float,3> min = {-1,-1,-1};
    int byte_offset = 0;
    int count = 0;
};

struct GltfBufferView {
    std::string buffer_idx= "";
    int byte_offset = 0;
    int byte_len = 0;
    int byte_stride = 0;
};

struct GltfTextureData {
    std::vector<uint8_t> data;
    int w , h;
};
struct GltfTexture {
    std::string sampler;
    std::string source;
    GltfTexture() {
        sampler = "";
        source = "";
    }
};

struct GltfPBRMaterial {
    std::array<float,3> base_color = {1,1,1};


    float metallic_factor = 1; 
    float roughness_factor = 1; 
    std::map<std::string,int> base_texture;
    std::map<std::string,int> metallic_roughness_texture;
    GltfPBRMaterial() {
        base_texture["index"] = -1;
        base_texture["texCoord"] = 0;

        metallic_roughness_texture["index"] = -1;
        metallic_roughness_texture["texCoord"] = 0;
    }
};
struct GltfMaterial {
    std::string diffuse = "";
    int shininess = 0;
    std::vector<float> specular;
    std::string technique_idx =  "";

    int base_texture_idx   = -1;
    int normal_texture_idx = -1;

    GltfPBRMaterial pbr_mat;
};

struct GltfProgram {
    std::string frag;
    std::string vert;
};

struct GltfTechnique {
    std::map<std::string,std::string> uniforms;
};


struct GltfObj {
    std::string main_scene_idx;
    GltfScene main_scene;

    std::map<std::string,GltfScene> scenes;
    std::map<std::string,GltfNode> nodes;
    std::map<std::string,GltfMesh> meshes;
    std::map<std::string,GltfAccessor> accessors;
    std::map<std::string,GltfBufferView> buffer_views;
    std::map<std::string,GltfTexture> textures;
    std::map<std::string,GltfMaterial> materials;
    std::map<std::string,GltfProgram> programs;
    std::map<std::string,GltfTechnique> techniques;

    std::string dir_path;
    std::map<std::string,std::vector<char>> buffers;
    std::map<std::string,GltfTextureData> textures_data;
};


class GltfLoader {
public:
    Json::Value json;
    std::vector<float> vertices;
    bool is_vertices_centered = true;

    std::vector<uint16_t> indices;
    std::vector<float> normals;
    std::vector<float> colors;
    std::vector<std::vector<float>> uv_coords;

    glm::mat4x4 main_transformation;
    GltfObj gltf_obj;


    GltfLoader(const char* gltf_path);
    void parse_meshes();
    void parse_nodes();
    void parse_accessor();
    void parse_buffer_views();
    void parse_and_load_buffers(std::string gltf_path);
    void parse_materials();
    void parse_texture();
    void extract_dir_path(std::string gltf_path);
    void parse_scenes_and_main_scene();
    void load_textures_data();



    void parse_node(GltfNode node);


    ~GltfLoader();
}; 