#pragma once

#include <fstream>
#include "json.h"


struct GltfMesh {
    int normal_idx , position_idx ,indices_idx;

    GltfMesh() {
        normal_idx = -1;
        position_idx = -1;
        indices_idx = -1;
    }
};
struct GltfNode {
    std::vector<int> children;
    int mesh_idx;  

    GltfNode() {
        mesh_idx = -1;
    }
};
struct GltfScene {
    std::vector<int> nodes;
};

struct GltfAccessor {
    int buffer_view_idx;
    int byte_offset;
    int count;

    GltfAccessor() {
        count = -1;
        buffer_view_idx = -1;
        byte_offset = 0;
    }
};

struct GltfBufferView {
    int buffer_idx;
    int byte_offset;
    int byte_len;

    GltfBufferView() {
        buffer_idx = -1;
        byte_offset = -1;
        byte_len = -1;        
    }    
};

struct GltfObj {
    GltfScene main_scene;

    std::map<std::string,GltfScene> scenes;
    std::vector<GltfNode> nodes;
    std::vector<GltfMesh> meshes;
    std::vector<GltfAccessor> accessors;
    std::vector<GltfBufferView> buffer_views;


    std::string dir_path;
    std::vector<std::vector<char>> buffers;
};


class GltfLoader {
public:
    Json::Value json;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    std::vector<float> normals;

    GltfObj gltf_obj;


    GltfLoader(const char* gltf_path,const char* gltf_bin);
    void parse_node(GltfNode node);

    ~GltfLoader();
}; 