#include "texture.hpp"
#include "pch.hpp"

Texture::Texture(const char* fp,int out_comps) {
    glGenTextures(1,&ID);

    path = fp;

    glBindTexture(GL_TEXTURE_2D,ID);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_R,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    int width , height;
    uint8_t* tex_data = stbi_load(fp,&width,&height,0,out_comps);    
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,tex_data);
    stbi_image_free(tex_data);
}

Texture::~Texture() {

}

void Texture::bind(int placement) {
    glActiveTexture(GL_TEXTURE0 + placement);
    glBindTexture(GL_TEXTURE_2D,ID);
}