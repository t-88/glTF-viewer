#include "gltf_loader.hpp"

GltfLoader::GltfLoader(const char *gltf_path, const char *gltf_bin)
{
    std::ifstream json_file(gltf_path, std::ios::binary);
    json_file >> json;
    json_file.close();

    std::ifstream bin_file(gltf_bin, std::ios::binary | std::ios::ate);
    size_t bin_len = bin_file.tellg();
    bin_file.seekg(0);
    std::vector<char> bin_char(bin_len);
    bin_file.read(bin_char.data(), bin_len);
    bin_file.close();

    Json::Value &primitive = json["meshes"][0]["primitives"][0];

    Json::Value &position_accessor = json["accessors"][primitive["attributes"]["POSITION"].asInt()];
    Json::Value &buffer_view = json["bufferViews"][position_accessor["bufferView"].asInt()];
    float *pos_buf = (float *)(bin_char.data() + buffer_view["byteOffset"].asInt() + position_accessor["byteOffset"].asInt());
    for (size_t i = 0; i < position_accessor["count"].asInt(); i++) {
        vertices.push_back(pos_buf[i * 3 + 0]);
        vertices.push_back(pos_buf[i * 3 + 1]);
        vertices.push_back(pos_buf[i * 3 + 2]);
    }

    Json::Value &indices_accessor = json["accessors"][0];
    Json::Value &indices_buffer_view = json["bufferViews"][indices_accessor["bufferView"].asInt()];
    uint16_t *indices_buf = (uint16_t *)(bin_char.data() + indices_buffer_view["byteOffset"].asInt());
    for (size_t i = 0; i < indices_accessor["count"].asInt(); i++) {
        indices.push_back(indices_buf[i]);
    }


    Json::Value &normals_accessor = json["accessors"][primitive["attributes"]["NORMAL"].asInt()];
    Json::Value &normals_buffer_view = json["bufferViews"][normals_accessor["bufferView"].asInt()];
    float *normals_buf = (float *)(bin_char.data() + normals_buffer_view["byteOffset"].asInt() + normals_accessor["byteOffset"].asInt());
    for (size_t i = 0; i < normals_accessor["count"].asInt(); i++) {
        normals.push_back(normals_buf[i * 3 + 0]);
        normals.push_back(normals_buf[i * 3 + 1]);
        normals.push_back(normals_buf[i * 3 + 2]);
    }


}


GltfLoader::~GltfLoader(){}