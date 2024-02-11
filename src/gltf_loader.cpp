#include <cmath>
#include <assert.h>

#include "gltf_loader.hpp"
#include "stb_image.h"
#include "stb_image_resize.h"

void GltfLoader::parse_node(GltfNode node) {
    if(node.mesh_idx.size() != 0) {
        GltfMesh mesh = gltf_obj.meshes[node.mesh_idx];
        GltfAccessor pos_accessor = gltf_obj.accessors[mesh.position_idx];
        GltfBufferView pos_buffer_view = gltf_obj.buffer_views[pos_accessor.buffer_view_idx];
        float* pos_buf = (float*)(gltf_obj.buffers[pos_buffer_view.buffer_idx].data() + 
                                  pos_accessor.byte_offset + 
                                  pos_buffer_view.byte_offset); 

        // data is interleaved
        int stride_offset = 0;
        if(pos_buffer_view.byte_stride > sizeof(float) * 3) {
            stride_offset = (pos_buffer_view.byte_stride - sizeof(float) * 3) / sizeof(float);
        }
        for (size_t i = 0; i < pos_accessor.count; i++) {
            vertices.push_back(pos_buf[i * 3 + 0 + i * stride_offset]);
            vertices.push_back(pos_buf[i * 3 + 1 + i * stride_offset]);
            vertices.push_back(pos_buf[i * 3 + 2 + i * stride_offset]);
        }

        GltfAccessor normal_accessor = gltf_obj.accessors[mesh.normal_idx];
        GltfBufferView normal_buffer_view = gltf_obj.buffer_views[normal_accessor.buffer_view_idx];
        float* normals_buf = (float*)(gltf_obj.buffers[normal_buffer_view.buffer_idx].data() + 
                                      normal_accessor.byte_offset + 
                                      normal_buffer_view.byte_offset); 
        
        stride_offset = 0;
        if(normal_buffer_view.byte_stride > sizeof(float) * 3) {
            stride_offset = (normal_buffer_view.byte_stride - sizeof(float) * 3) / sizeof(float);
        }
        for (size_t i = 0; i < normal_accessor.count; i++) {
            normals.push_back(normals_buf[i * 3 + 0 + i * stride_offset]);
            normals.push_back(normals_buf[i * 3 + 1 + i * stride_offset]);
            normals.push_back(normals_buf[i * 3 + 2 + i * stride_offset]);
        }

        if(mesh.indices_idx.size() != 0) {
            GltfAccessor indices_accessor = gltf_obj.accessors[mesh.indices_idx];
            GltfBufferView indices_buffer_view = gltf_obj.buffer_views[indices_accessor.buffer_view_idx];
            uint16_t* indices_buf = (uint16_t*)(gltf_obj.buffers[indices_buffer_view.buffer_idx].data() + 
                                                indices_accessor.byte_offset + 
                                                indices_buffer_view.byte_offset); 
            for (size_t i = 0; i < indices_accessor.count; i++) {
                indices.push_back(indices_buf[i]);
            }
        }

        // only deal with one texture
        if(mesh.texture_idx.size() != 0) {
            GltfAccessor texture_accessor = gltf_obj.accessors[mesh.texture_idx[0]];
            GltfBufferView texture_buffer_view = gltf_obj.buffer_views[texture_accessor.buffer_view_idx];
            float* texture_buf = (float*)(gltf_obj.buffers[texture_buffer_view.buffer_idx].data() + 
                                          texture_accessor.byte_offset + 
                                          texture_buffer_view.byte_offset); 
            std::vector<float> uv_coord;
            for (size_t i = 0; i < texture_accessor.count; i++) {
                uv_coord.push_back(texture_buf[2 * i + 0]);
                uv_coord.push_back(texture_buf[2 * i + 1]);
            }            
            uv_coords.push_back(uv_coord);
        }
    }

    for (size_t i = 0; i < node.children.size(); i++) {
        parse_node(gltf_obj.nodes[node.children[i]]);
    }
}


void GltfLoader::parse_meshes() {
    if(json["meshes"].isArray()) {

        for (int i = 0; i < json["meshes"].size(); i++) {
            GltfMesh mesh;
            if(json["meshes"][i]["primitives"][0]["attributes"].isMember("NORMAL")) {
                mesh.normal_idx =  std::to_string(json["meshes"][i]["primitives"][0]["attributes"]["NORMAL"].asInt());
            }
            if(json["meshes"][i]["primitives"][0]["attributes"].isMember("POSITION")) { 
                mesh.position_idx = std::to_string(json["meshes"][i]["primitives"][0]["attributes"]["POSITION"].asInt());
            }

            // just handle one texture as a start 
            if(json["meshes"][i]["primitives"][0]["attributes"].isMember("TEXCOORD_0")) { 
                mesh.texture_idx.push_back(std::to_string(json["meshes"][i]["primitives"][0]["attributes"]["TEXCOORD_0"].asInt()));
            }  

            if(json["meshes"][i]["primitives"][0].isMember("indices")) { 
                mesh.indices_idx = std::to_string(json["meshes"][i]["primitives"][0]["indices"].asInt());
            }
            gltf_obj.meshes[std::to_string(i)] = mesh;
        }
    } else {
        for (int i = 0; i < json["meshes"].size(); i++) {
            GltfMesh mesh;
            std::string key = json["meshes"].getMemberNames()[i];

            if(json["meshes"][key]["primitives"][0]["attributes"].isMember("NORMAL")) {
                mesh.normal_idx = json["meshes"][key]["primitives"][0]["attributes"]["NORMAL"].asString();
            }
            if(json["meshes"][key]["primitives"][0]["attributes"].isMember("POSITION")) { 
                mesh.position_idx = json["meshes"][key]["primitives"][0]["attributes"]["POSITION"].asString();
            }

            // just handle one texture as a start 
            if(json["meshes"][key]["primitives"][0]["attributes"].isMember("TEXCOORD_0")) { 
                mesh.texture_idx.push_back(json["meshes"][key]["primitives"][0]["attributes"]["TEXCOORD_0"].asString());
            }   

            if(json["meshes"][key]["primitives"][0].isMember("indices")) { 
                mesh.indices_idx = json["meshes"][key]["primitives"][0]["indices"].asString();
            }
            gltf_obj.meshes[key] = mesh;
        }
    }



}
void GltfLoader::parse_nodes() { 
    if(json["nodes"].isArray()) {
        for (int i = 0; i < json["nodes"].size(); i++) {
            GltfNode node;
            // children 
            if(json["nodes"][i].isMember("children")) {
                for (int j = 0; j < json["nodes"][i]["children"].size(); j++) {
                    node.children.push_back(json["nodes"][i]["children"][j].asString());
                }
            }
            // mesh
            if(json["nodes"][i].isMember("mesh")) { 
                node.mesh_idx = json["nodes"][i]["mesh"].asString();
            }
            gltf_obj.nodes[std::to_string(i)] = node;
        }
    } else {
        for (int i = 0; i < json["nodes"].size(); i++) {
            GltfNode node;
            std::string key = json["nodes"].getMemberNames()[i];

            // children 
            if(json["nodes"][key].isMember("children")) {
                for (int j = 0; j < json["nodes"][key]["children"].size(); j++) {
                    node.children.push_back(json["nodes"][key]["children"][j].asString());
                }
            }
            // mesh
            if(json["nodes"][key].isMember("mesh")) { 
                node.mesh_idx = json["nodes"][key]["mesh"].asString(); 
            } else if(json["nodes"][key].isMember("meshes")) { 
                node.mesh_idx = json["nodes"][key]["meshes"][0].asString();
            } 

            gltf_obj.nodes[key] = node;
        }
    }
}
void GltfLoader::parse_accessor() {
    if(json["accessors"].isArray()) { 
        for (int i = 0; i < json["accessors"].size(); i++) {
            GltfAccessor accessor;

            accessor.buffer_view_idx = json["accessors"][i]["bufferView"].asString();
            accessor.byte_offset = json["accessors"][i]["byteOffset"].asInt();
            accessor.count = json["accessors"][i]["count"].asInt();

            gltf_obj.accessors[std::to_string(i)] = accessor;
        }
    } else {
        for (int i = 0; i < json["accessors"].size(); i++) {
            GltfAccessor accessor;
            std::string key = json["accessors"].getMemberNames()[i];

            accessor.buffer_view_idx = json["accessors"][key]["bufferView"].asString();
            accessor.byte_offset = json["accessors"][key]["byteOffset"].asInt();
            accessor.count = json["accessors"][key]["count"].asInt();

            gltf_obj.accessors[key] = accessor;
        }        
    }
}
void GltfLoader::parse_buffer_views() {
    if(json["bufferViews"].isArray()) {  
        for (int i = 0; i < json["bufferViews"].size(); i++) {

            GltfBufferView buffer_view;
    
            buffer_view.buffer_idx = json["bufferViews"] [i]["buffer"].asString();
            buffer_view.byte_offset = json["bufferViews"][i]["byteOffset"].asInt();
            buffer_view.byte_len = json["bufferViews"]   [i]["byteLength"].asInt();

            if(json["bufferViews"][i].isMember("byteStride")) {
                buffer_view.byte_stride = json["bufferViews"][i]["byteStride"].asInt();
            }
            
            gltf_obj.buffer_views[std::to_string(i)] = buffer_view;
    
        }
    } else {
        for (int i = 0; i < json["bufferViews"].size(); i++) {
            GltfBufferView buffer_view;
            std::string key = json["bufferViews"].getMemberNames()[i];
    
            buffer_view.buffer_idx = json["bufferViews"] [key]["buffer"].asString();
            buffer_view.byte_offset = json["bufferViews"][key]["byteOffset"].asInt();
            buffer_view.byte_len = json["bufferViews"]   [key]["byteLength"].asInt();

            if(json["bufferViews"][i].isMember("byteStride")) {
                buffer_view.byte_stride = json["bufferView"][i]["byteStride"].asInt();
            }            
            
            gltf_obj.buffer_views[key] = buffer_view;
    
        }        
    }
}

void GltfLoader::parse_and_load_buffers(std::string gltf_path) {
    extract_dir_path(gltf_path);

    if(json["buffers"].isArray()) {  
        for (int i = 0; i < json["buffers"].size(); i++) {
            std::string uri = json["buffers"][i]["uri"].asString();
            int buffer_len = json["buffers"][i]["byteLength"].asInt();

            std::ifstream bin_file(gltf_obj.dir_path + uri, std::ios::binary);
                std::vector<char> bin_char(buffer_len);
                bin_file.read(bin_char.data(),buffer_len);
            bin_file.close();

            gltf_obj.buffers[std::to_string(i)] =  bin_char;
        }
    } else {
        for (int i = 0; i < json["buffers"].size(); i++) {
            std::string key = json["buffers"].getMemberNames()[i];

            std::string uri = json["buffers"][key]["uri"].asString();
            int buffer_len = json["buffers"][key]["byteLength"].asInt();
    

            std::ifstream bin_file(gltf_obj.dir_path + uri, std::ios::binary);
                std::vector<char> bin_char(buffer_len);
                bin_file.read(bin_char.data(),buffer_len);
            bin_file.close();

            gltf_obj.buffers[key] =  bin_char;
        }
    }
}


void GltfLoader::extract_dir_path(std::string gltf_path) {
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
}
void GltfLoader::parse_scenes_and_main_scene() {
    if(json["scenes"].isArray()) {
        for (int i = 0; i < json["scenes"].size(); i++) {
            GltfScene scene;
            if(json["scenes"][i].isMember("nodes")) {
                for (int j = 0; j < json["scenes"][i]["nodes"].size(); j++) {
                    scene.nodes[std::to_string(j)] = json["scenes"][i]["nodes"][j].asString();
                }
            }
            gltf_obj.scenes[std::to_string(i)] = scene; 
        }    
    } else {
        for (int i = 0; i < json["scenes"].size(); i++) {
            GltfScene scene;
            std::string key = json["scenes"].getMemberNames()[i];
            if(json["scenes"][key].isMember("nodes")) {
                for (int j = 0; j < json["scenes"][key]["nodes"].size(); j++) {
                    scene.nodes[std::to_string(j)] = json["scenes"][key]["nodes"][j].asString();
                }
            }
            gltf_obj.scenes[key] = scene; 
        }    
    }
    gltf_obj.main_scene = gltf_obj.scenes[json["scene"].asString()];

}


std::string replace_all(std::string str,std::string find,std::string replace_with) {
    int found_idx;
    while((found_idx = str.find(find)) && (found_idx != std::string::npos)) {
        str.replace(found_idx,find.size(),replace_with);
    }
    return str;
}

void GltfLoader::load_textures_data() {
    if(!json.isMember("images")) return;

    for (int i = 0; i < json["images"].size(); i++) {
        std::string key;
        if(json["images"].isArray()) { key = std::to_string(i); } 
        else { key = json["images"].getMemberNames()[i]; }

        std::string uri;
        if(json["images"].isArray()) {
            uri = gltf_obj.dir_path + json["images"][i]["uri"].asString();
        } else {
            uri = gltf_obj.dir_path + json["images"][key]["uri"].asString();
        }
        // handle spaces
        uri = replace_all(uri,"%20"," ");

        GltfTextureData texture_data;
        uint8_t* img_data =  stbi_load(uri.c_str(),&texture_data.w,&texture_data.h,0,3);

        if(stbi_failure_reason()) {
            printf("%s\n",stbi_failure_reason());
            assert(img_data != 0 && "stbi Failed to load img\n");
        }


        int power_of_2_width  = pow(2,ceil(log2(texture_data.w))); 
        int power_of_2_height = pow(2,ceil(log2(texture_data.h)));
        if(power_of_2_width != texture_data.w || power_of_2_height != texture_data.h) {
            uint8_t* resized_img_data = (uint8_t*) malloc(power_of_2_width * power_of_2_height * 3);
            stbir_resize_uint8(img_data,texture_data.w,texture_data.h,0,resized_img_data,power_of_2_width,power_of_2_height,0,3);
            texture_data.w = power_of_2_width;
            texture_data.h = power_of_2_height;
            
            stbi_image_free(img_data);
            img_data = resized_img_data;
        }

        texture_data.data =  std::vector<uint8_t>(img_data,img_data + texture_data.w * texture_data.h * 3);
        gltf_obj.textures_data[key] = texture_data;
        stbi_image_free(img_data);            
    }
}

void GltfLoader::parse_materials() {
// material
    if(!json.isMember("materials")) return;

    if(json["materials"].isArray()) {
        for (int i = 0; i < json["materials"].size(); i++) {
            GltfMaterial mat;
            // NOTE: in pbr i am not using metallicRoughnessTexture
            if(json["materials"][i].isMember("pbrMetallicRoughness")) {
                if(json["materials"][i]["pbrMetallicRoughness"].isMember("baseColorFactor")) {
                    for (int j = 0; j < 4; j++) {
                        mat.pbr_mat.base_color[i] = json["materials"][i]["pbrMetallicRoughness"]["baseColorFactor"][j].asFloat(); 
                    }
                }
                if(json["materials"][i]["pbrMetallicRoughness"].isMember("baseColorTexture")) { 
                    mat.pbr_mat.texture["index"] = json["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"]["index"].asInt();
                    if(json["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"].isMember("texCoord")) {
                        mat.pbr_mat.texture["texCoord"] = json["materials"][i]["pbrMetallicRoughness"]["baseColorTexture"]["texCoord"].asInt();
                    }
                }
                if(json["materials"][i]["pbrMetallicRoughness"].isMember("metallicFactor")) { 
                    mat.pbr_mat.metallic_factor = json["materials"][i]["pbrMetallicRoughness"]["metallicFactor"].asFloat();
                }
                if(json["materials"][i]["pbrMetallicRoughness"].isMember("roughnessFactor")) { 
                    mat.pbr_mat.roughness_factor = json["materials"][i]["pbrMetallicRoughness"]["roughnessFactor"].asFloat();
                }
            }
        }
    } 
    
}
void  GltfLoader::parse_texture() {
    // textures
    if(json.isMember("textures")) return;
    if(json["textures"].isArray()) {
        for (int i = 0; i < json["textures"].size(); i++) {
            GltfTexture texture;
            texture.sampler = gltf_obj.dir_path + json["textures"][i]["sampler"].asString();
            texture.source = gltf_obj.dir_path + json["textures"][i]["source"].asString();
            gltf_obj.textures[std::to_string(i)] = texture;
        }            
    } else {
        for (int i = 0; i < json["textures"].size(); i++) {
            std::string key = json["textures"].getMemberNames()[i];
            GltfTexture texture;
            texture.sampler = gltf_obj.dir_path + json["textures"][key]["sampler"].asString();
            texture.source = gltf_obj.dir_path + json["textures"][key]["source"].asString();
            gltf_obj.textures[key] = texture;
        }
    }
}



GltfLoader::GltfLoader(const char *gltf_path)
{
    // reading the gltf and bin files
    std::ifstream json_file(gltf_path, std::ios::binary);
    json_file >> json;
    json_file.close();



    parse_meshes();   
    parse_nodes();
    parse_accessor();
    parse_buffer_views();
    
    parse_and_load_buffers(gltf_path);
    parse_scenes_and_main_scene();
    load_textures_data();
    
    parse_materials();
    parse_texture();


    
    GltfNode main_node = gltf_obj.nodes[gltf_obj.main_scene.nodes["0"]];
    parse_node(main_node);
}


GltfLoader::~GltfLoader(){}