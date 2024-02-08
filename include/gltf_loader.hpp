#pragma once

#include <fstream>
#include "json.h"

class GltfLoader {
public:
    Json::Value json;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    GltfLoader(const char* gltf_path,const char* gltf_bin);
    ~GltfLoader();
}; 