#include "gltf_loader.hpp"

void GltfLoader::parse_node(GltfNode node) {
    if(node.mesh_idx != -1) {

        GltfMesh mesh = gltf_obj.meshes[node.mesh_idx];
        GltfAccessor pos_accessor = gltf_obj.accessors[mesh.position_idx];
        GltfBufferView pos_buffer_view = gltf_obj.buffer_views[pos_accessor.buffer_view_idx];
        float* pos_buf = (float*)(gltf_obj.buffers[pos_buffer_view.buffer_idx].data() + pos_accessor.byte_offset + pos_buffer_view.byte_offset); 
        for (size_t i = 0; i < pos_accessor.count; i++) {
            vertices.push_back(pos_buf[i * 3 + 0]);
            vertices.push_back(pos_buf[i * 3 + 1]);
            vertices.push_back(pos_buf[i * 3 + 2]);
        }

        GltfAccessor normal_accessor = gltf_obj.accessors[mesh.normal_idx];
        GltfBufferView normal_buffer_view = gltf_obj.buffer_views[normal_accessor.buffer_view_idx];
        float* normals_buf = (float*)(gltf_obj.buffers[normal_buffer_view.buffer_idx].data() + normal_accessor.byte_offset + normal_buffer_view.byte_offset); 
        for (size_t i = 0; i < normal_accessor.count; i++) {
            normals.push_back(normals_buf[i * 3 + 0]);
            normals.push_back(normals_buf[i * 3 + 1]);
            normals.push_back(normals_buf[i * 3 + 2]);
        }

        GltfAccessor indices_accessor = gltf_obj.accessors[mesh.indices_idx];
        GltfBufferView indices_buffer_view = gltf_obj.buffer_views[indices_accessor.buffer_view_idx];
        uint16_t* indices_buf = (uint16_t*)(gltf_obj.buffers[indices_buffer_view.buffer_idx].data() + indices_accessor.byte_offset + indices_buffer_view.byte_offset); 
        for (size_t i = 0; i < indices_accessor.count; i++) {
            indices.push_back(indices_buf[i]);
        }
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        parse_node(gltf_obj.nodes[node.children[i]]);
    }
}

GltfLoader::GltfLoader(const char *gltf_path, const char *gltf_bin)
{
    // reading the gltf and bin files
    std::ifstream json_file(gltf_path, std::ios::binary);
    json_file >> json;
    json_file.close();




    // parsing
    // meshes
    for (int i = 0; i < json["meshes"].size(); i++) {
        GltfMesh mesh;
        
        if(json["meshes"][i]["primitives"][0]["attributes"].isMember("NORMAL")) {
            mesh.normal_idx = json["meshes"][i]["primitives"][0]["attributes"]["NORMAL"].asInt();
        }
        if(json["meshes"][i]["primitives"][0]["attributes"].isMember("POSITION")) { 
            mesh.position_idx = json["meshes"][i]["primitives"][0]["attributes"]["POSITION"].asInt();
        }
        if(json["meshes"][i]["primitives"][0].isMember("indices")) { 
            mesh.indices_idx = json["meshes"][i]["primitives"][0]["indices"].asInt();
        }

        gltf_obj.meshes.push_back(mesh);
    }
    
    // nodes
    for (int i = 0; i < json["nodes"].size(); i++) {
        GltfNode node;
        
        // children 
        if(json["nodes"][i].isMember("children")) {
            for (int j = 0; j < json["nodes"][i]["children"].size(); j++) {
                node.children.push_back(json["nodes"][i]["children"][j].asInt());
            }
        }
        // mesh
        if(json["nodes"][i].isMember("mesh")) { 
            node.mesh_idx = json["nodes"][i]["mesh"].asInt();
        }

        gltf_obj.nodes.push_back(node);
    }
     
    // accessors
    for (int i = 0; i < json["accessors"].size(); i++) {
        GltfAccessor accessor;

        accessor.buffer_view_idx = json["accessors"][i]["bufferView"].asInt();
        accessor.byte_offset = json["accessors"][i]["byteOffset"].asInt();
        accessor.count = json["accessors"][i]["count"].asInt();

        gltf_obj.accessors.push_back(accessor);
    }
    
    // buffer views
    for (int i = 0; i < json["bufferViews"].size(); i++) {
        GltfBufferView buffer_view;

        buffer_view.buffer_idx = json["bufferViews"] [i]["buffer"].asInt();
        buffer_view.byte_offset = json["bufferViews"][i]["byteOffset"].asInt();
        buffer_view.byte_len = json["bufferViews"]   [i]["byteLength"].asInt();
        
        gltf_obj.buffer_views.push_back(buffer_view);

    }

    // buffers only supports bin files
    
    // get gltf dir, needed when handling buffers
    std::string gltf_path_str = std::string(gltf_path);
    int last_slash = gltf_path_str.find_last_of("/");
    if(last_slash != -1) {
        gltf_obj.dir_path = gltf_path_str.substr(0,last_slash + 1);
    } else {
        // no slash
        gltf_obj.dir_path = "";
    }


    for (int i = 0; i < json["buffers"].size(); i++) {
        std::string uri = json["buffers"][i]["uri"].asString();
        int buffer_len = json["buffers"][i]["byteLength"].asInt();

        std::ifstream bin_file(gltf_bin, std::ios::binary);
            std::vector<char> bin_char(buffer_len);
            bin_file.read(bin_char.data(),buffer_len);
        bin_file.close();

        gltf_obj.buffers.push_back(bin_char);
    }
    

    // only handling if the main scene is int 
    // for now :) 
    if(json["scene"].isInt()) {
        // scenes
        for (int i = 0; i < json["scenes"].size(); i++) {
            GltfScene scene;
            if(json["scenes"][i].isMember("nodes")) { 
                for (int j = 0; j < json["scenes"][i]["nodes"].size(); j++) {
                    scene.nodes.push_back(json["scenes"][i]["nodes"][j].asInt());
                }

            }
            gltf_obj.scenes[std::to_string(i)] = scene; 
        }
        gltf_obj.main_scene = gltf_obj.scenes[json["scene"].asString()];
    } else {
        printf("Failed to prase gltf\n");
        printf("What is the main scene idx is a key\n");
        exit(-1);
    }
    
    
    GltfNode main_node = gltf_obj.nodes[gltf_obj.main_scene.nodes[0]];
    parse_node(main_node);
}


GltfLoader::~GltfLoader(){}