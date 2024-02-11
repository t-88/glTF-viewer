#include <assert.h>
#include "mesh.hpp"


Mesh::Mesh(GltfLoader gltf_loader) {
    setup_transformations();
    setup_shader(gltf_loader);
    setup_vertices(gltf_loader);
}
Mesh::~Mesh() {}

void Mesh::setup_transformations() {
    rotation = glm::vec3(0.);
    rotation_mat = glm::mat4x4(1.);
    scale =glm::vec3(0.25);
    scale_mat = glm::scale(glm::mat4x4(1.),scale);
    translate = glm::vec3(0.,0.,-1.);
    translation_mat = glm::translate(glm::mat4x4(1.),translate);
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
void Mesh::setup_vertices(GltfLoader gltf)  {
    vertices_count = gltf.vertices.size();
    assert(vertices_count != 0 && "mesh dosnt have vertices");

    indices = gltf.indices;

    std::vector<float> normals;

    // generate normals if none provided 
    if(gltf.normals.size() == 0) {
        // no indices array provided
        if(indices.size() == 0) {
            for (size_t i = 0; i < gltf.vertices.size() / 9; i++) {
                glm::vec3 a(gltf.vertices[9 * i + 0],gltf.vertices[9 * i + 1],gltf.vertices[9 * i + 2]);
                glm::vec3 b(gltf.vertices[9 * i + 3],gltf.vertices[9 * i + 4],gltf.vertices[9 * i + 5]);
                glm::vec3 c(gltf.vertices[9 * i + 6],gltf.vertices[9 * i + 7],gltf.vertices[9 * i + 8]);

                printf("%ld:\n",i);
                printf("%f %f %f\n",a.x,a.y,a.z);
                printf("%f %f %f\n",b.x,b.y,b.z);
                printf("%f %f %f\n",c.x,c.y,c.z);
                printf("\n");

                glm::vec3 ab = a - b;
                glm::vec3 ac = a - c;

                glm::vec3 normal = glm::cross(ab,ac);

                for (size_t i = 0; i < 3; i++) {
                    normals.push_back(normal.x);
                    normals.push_back(normal.y);
                    normals.push_back(normal.z);
                }
            }
        } else {
            int traingle_count = indices.size() / 3;

            normals = std::vector<float>(vertices_count);
            for (size_t i = 0; i < traingle_count; i++) {
                int a_idx = indices[3 * i + 0];
                int b_idx = indices[3 * i + 1];
                int c_idx = indices[3 * i + 2];

                glm::vec3 a(gltf.vertices[3 * a_idx + 0],gltf.vertices[3 * a_idx + 1],gltf.vertices[3 * a_idx + 2]);
                glm::vec3 b(gltf.vertices[3 * b_idx + 0],gltf.vertices[3 * b_idx + 1],gltf.vertices[3 * b_idx + 2]);
                glm::vec3 c(gltf.vertices[3 * c_idx + 0],gltf.vertices[3 * c_idx + 1],gltf.vertices[3 * c_idx + 2]);
            
                glm::vec3 ab = a - b;
                glm::vec3 ac = a - c;

                glm::vec3 normal = glm::cross(ab,ac);
                for (size_t i = 0; i < 3; i++) {
                    normals[3 * a_idx + i] = normal[i];
                    normals[3 * b_idx + i] = normal[i];
                    normals[3 * c_idx + i] = normal[i];
                }
            }
        }
    } else {
        normals = gltf.normals;
    }


    // create a the buffer
    for (size_t i = 0; i < gltf.vertices.size() / 3; i++) {
        buffer.push_back(gltf.vertices[3 * i + 0]);        
        buffer.push_back(gltf.vertices[3 * i + 1]);        
        buffer.push_back(gltf.vertices[3 * i + 2]);        
        
        buffer.push_back(normals[3 * i + 0]);        
        buffer.push_back(normals[3 * i + 1]);        
        buffer.push_back(normals[3 * i + 2]);   

        for (size_t j = 0; j < gltf.uv_coords.size(); j++) {
            buffer.push_back(gltf.uv_coords[j][2 * i + 0]);
            buffer.push_back(gltf.uv_coords[j][2 * i + 1]);
        }
    }



    // we count 6 for vertices and normals  
    int vertex_data_offset = 6;
    // we add uv_coords
    vertex_data_offset += 2 * gltf.uv_coords.size(); 

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(buffer[0]), buffer.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_data_offset, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_data_offset, (void*)( 3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // point to the texture uv
    for (size_t i = 0; i < gltf.uv_coords.size(); i++) {
        glVertexAttribPointer(2 + i, 2, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_data_offset, (void*)( 6 + (2 * i) * sizeof(float)));
        glEnableVertexAttribArray(2 + i);    
    }

    if(indices.size() != 0) {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::setup_textures(std::map<std::string,GltfTextureData> texture_data) {
    // glGenTextures(1,&texture0);
    // glBindTexture(GL_TEXTURE_2D,texture0);


    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture_data["0"].w,texture_data["0"].h,0,GL_RGB,GL_UNSIGNED_BYTE,texture_data["0"].data.data());
    // glGenerateMipmap(GL_TEXTURE_2D);
}


void Mesh::render() {
    glBindVertexArray(vao);

    // if(texture0 != -1) {
    //     glBindTexture(GL_TEXTURE_2D,texture0);
    // }

    if(indices.size() != 0) {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, indices.data());
    } else {
        glDrawArrays(GL_TRIANGLES,0,vertices_count);
    }
}