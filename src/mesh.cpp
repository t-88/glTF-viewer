#include "mesh.hpp"

Mesh::Mesh(std::vector<float> _vertices, std::vector<uint16_t> _indices)
{
    
    vertices = _vertices;
    indices = _indices;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


Mesh::Mesh(std::vector<float> _vertices,std::vector<float> _normals, std::vector<uint16_t> _indices,std::vector<std::vector<float>> textures)
{
    vertices = _vertices;
    normals = _normals;
    indices = _indices;

    for (size_t i = 0; i < vertices.size() / 3; i++) {
        buffer.push_back(vertices[3 * i + 0]);        
        buffer.push_back(vertices[3 * i + 1]);        
        buffer.push_back(vertices[3 * i + 2]);        
        
        buffer.push_back(normals[3 * i + 0]);        
        buffer.push_back(normals[3 * i + 1]);        
        buffer.push_back(normals[3 * i + 2]);     
    }
    



    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), buffer.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)( 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    if(indices.size() != 0) {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    uint texture0;
    glGenTextures(1,&texture0);
    glBindTexture(GL_TEXTURE_2D,texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,)



}



Mesh::~Mesh() {}

void Mesh::render() {
    glBindVertexArray(vao);

    if(indices.size() != 0) {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, indices.data());
    } else {
        glDrawArrays(GL_TRIANGLES,0,vertices.size());
    }
}