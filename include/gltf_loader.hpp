#pragma once

#include <fstream>
#include "json.h"


struct GltfMesh {
    std::string normal_idx , position_idx ,indices_idx;

    GltfMesh() {
        normal_idx = "";
        position_idx = "";
        indices_idx = "";
    }
};
struct GltfNode {
    std::vector<std::string> children;
    std::string mesh_idx;  
    GltfNode() {
        mesh_idx = "";
    }
};
struct GltfScene {
    std::map<std::string, std::string> nodes;
};

struct GltfAccessor {
    std::string buffer_view_idx;
    int byte_offset;
    int count;

    GltfAccessor() {
        count = -1;
        buffer_view_idx = "";
        byte_offset = 0;
    }
};

struct GltfBufferView {
    std::string buffer_idx;
    int byte_offset;
    int byte_len;

    GltfBufferView() {
        buffer_idx = "";
        byte_offset = -1;
        byte_len = -1;        
    }    
};

struct GltfObj {
    std::string main_scene_idx;
    GltfScene main_scene;

    std::map<std::string,GltfScene> scenes;
    std::map<std::string,GltfNode> nodes;
    std::map<std::string,GltfMesh> meshes;
    std::map<std::string,GltfAccessor> accessors;
    std::map<std::string,GltfBufferView> buffer_views;


    std::string dir_path;
    std::map<std::string,std::vector<char>> buffers;
    std::map<std::string,std::vector<uint8_t>> textures;


};


class GltfLoader {
public:
    Json::Value json;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    std::vector<float> normals;


    GltfObj gltf_obj;


    GltfLoader(const char* gltf_path);
    void parse_meshes();
    void parse_nodes();
    void parse_accessor();
    void parse_buffer_views();
    void parse_and_load_buffers(std::string gltf_path);
    void extract_dir_path(std::string gltf_path);
    void parse_scenes_and_main_scene();
    void load_textures();
    void parse_textures_coords();



    void parse_node(GltfNode node);


    ~GltfLoader();
}; 