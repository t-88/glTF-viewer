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
Mesh::Mesh(GltfLoader gltf_loader) {
    setup_transformations();
    setup_shader(gltf_loader);
    setup_vertices(gltf_loader.vertices,gltf_loader.normals,gltf_loader.uv_coords[0],gltf_loader.indices);
    setup_textures(gltf_loader.gltf_obj.textures_data);
}
Mesh::~Mesh() {}

void Mesh::setup_vertices(std::vector<float> _vertices,std::vector<float> _normals, std::vector<uint16_t> _indices)  {
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
}
void Mesh::setup_vertices(std::vector<float> _vertices,std::vector<float> _normals,std::vector<float> texture_data, std::vector<uint16_t> _indices)  {
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

        buffer.push_back(texture_data[2 * i + 0]);  
        buffer.push_back(texture_data[2 * i + 1]);  
    }


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), buffer.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)( 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)( 6 * sizeof(float)));
    glEnableVertexAttribArray(2);    

    if(indices.size() != 0) {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Mesh::setup_textures(std::map<std::string,GltfTextureData> texture_data) {
    glGenTextures(1,&texture0);
    glBindTexture(GL_TEXTURE_2D,texture0);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture_data["0"].w,texture_data["0"].h,0,GL_RGB,GL_UNSIGNED_BYTE,texture_data["0"].data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
}
void Mesh::setup_shader(GltfLoader gltf_loader) {
    // TODO: its just temp for now, plz remove it 
    #define WIDTH 800
    #define GUI_WIDTH 200
    #define SCENE_WIDTH (WIDTH - GUI_WIDTH)
    #define HEIGHT 600
    
    glm::mat4 model = translation_mat * rotation_mat * scale_mat;
    glm::mat4 view = glm::mat4(1.);
    glm::mat4 proj = glm::perspective(glm::radians(45.f),(float)SCENE_WIDTH/HEIGHT,0.1f,100.f);
    glm::vec3 light_dir = glm::vec3(0.,0.,1.);
    
    shader = Shader("./shader.vert","./shader.frag");
    shader.enable();
    shader.set_mat4x4("proj",glm::value_ptr(proj));
    shader.set_mat4x4("view",glm::value_ptr(view));
    shader.set_mat4x4("model",glm::value_ptr(model));
    shader.set_vec3("light_dir",{light_dir.x,light_dir.y,light_dir.z});



}

void Mesh::setup_transformations() {
    rotation = glm::vec3(0.);
    rotation_mat = glm::mat4x4(1.);
    scale =glm::vec3(0.25);
    scale_mat = glm::scale(glm::mat4x4(1.),scale);
    translate = glm::vec3(0.,0.,-1.);
    translation_mat = glm::translate(glm::mat4x4(1.),translate);
}
void Mesh::render() {
    glBindVertexArray(vao);

    if(texture0 != -1) {
        glBindTexture(GL_TEXTURE_2D,texture0);
    }

    if(indices.size() != 0) {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, indices.data());
    } else {
        glDrawArrays(GL_TRIANGLES,0,vertices.size());
    }
}