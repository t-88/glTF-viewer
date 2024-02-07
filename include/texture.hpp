#pragma once
#include <stdint.h>
#include <string>

class Texture
{
public:
    uint32_t ID;
    std::string type;
    std::string path;

    Texture(const char* fp,int out_comps = 3);
    ~Texture();

    void bind(int placement = 0);
};

