#pragma once

#include <fstream>
#include "json.h"


struct GltfMesh {
    std::string normal_idx , position_idx ,indices_idx;
    std::vector<std::string> texture_idx;
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

struct GltfMaterial {
    std::string diffuse = "";
    int shininess = 0;
    std::vector<float> specular;
    std::string technique_idx =  "";    
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
    std::vector<uint16_t> indices;
    std::vector<float> normals;
    std::vector<std::vector<float>> textures;


    GltfObj gltf_obj;


    GltfLoader(const char* gltf_path);
    void parse_meshes();
    void parse_nodes();
    void parse_accessor();
    void parse_buffer_views();
    void parse_and_load_buffers(std::string gltf_path);
    void extract_dir_path(std::string gltf_path);
    void parse_scenes_and_main_scene();
    void load_textures_data();



    void parse_node(GltfNode node);


    ~GltfLoader();
}; 