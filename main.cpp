#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils.hpp"
#include "json.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define WIDTH 800
#define HEIGHT 600

static char shader_log[512];
static int shader_status;

std::vector<float> vert = {
    0.0, 0.4,  0.0,
    -0.5, -0.5, 0.0,
    0.5,  -0.5,  0.0,
};
std::vector<float> mesh_vertices;


class Mesh {
public:
    uint32_t vao;
    uint32_t vbo; 
    uint32_t ebo; 

    int vertices_count = 0;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    
    Mesh(std::vector<float> _vertices,std::vector<uint16_t> _indices,int _vertices_count): vertices_count(_vertices_count) {
        vertices = _vertices;
        indices = _indices;

        glGenVertexArrays(1,&vao);
        glGenBuffers(1,&vbo);
        glGenBuffers(1,&ebo);

        glBindVertexArray(vao);
    
        glBindBuffer(GL_ARRAY_BUFFER,vbo); 
        glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(vertices[0]),vertices.data(),GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float) * 3,0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(indices[0]),indices.data(),GL_STATIC_DRAW);


        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }
    ~Mesh() {}


    void render() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,indices.data());
    }

};

void imgui_init(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");
}



void on_window_resize(GLFWwindow* window, int w, int h) {}
int main() {
    Json::Value json;

    std::ifstream json_file("assets/box.gltf",std::ios::binary);
    json_file >> json;
    json_file.close();
    
    std::ifstream bin_file("assets/Box0.bin",std::ios::binary | std::ios::ate);
    size_t bin_len = bin_file.tellg();
    bin_file.seekg(0);
    std::vector<char> bin_char(bin_len);
    bin_file.read(bin_char.data(),bin_len);
    bin_file.close();
    
    Json::Value& primitive = json["meshes"][0]["primitives"][0];         
    Json::Value& position_accessor = json["accessors"][primitive["attributes"]["POSITION"].asInt()];
    Json::Value& buffer_view = json["bufferViews"][position_accessor["bufferView"].asInt()];
    float* buffer = (float*)(bin_char.data() + buffer_view["byteOffset"].asInt() + position_accessor["byteOffset"].asInt() );

    for (size_t i = 0; i < position_accessor["count"].asInt(); i++){
        mesh_vertices.push_back(buffer[i * 3 + 0]);
        mesh_vertices.push_back(buffer[i * 3 + 1]);
        mesh_vertices.push_back(0);
    }

    std::vector<uint16_t> mesh_indices;
    Json::Value& indices_accessor = json["accessors"][0];
    Json::Value& indices_buffer_view = json["bufferViews"][indices_accessor["bufferView"].asInt()];
    uint16_t* buf = (uint16_t*)(bin_char.data() + indices_buffer_view["byteOffset"].asInt());
    for (size_t i = 0; i < indices_accessor["count"].asInt(); i++){
        mesh_indices.push_back(buf[i]);
    }





    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);


    GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT,"42",NULL,NULL);
    
    if(window == NULL) {
        ERROR("failed to create window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ERROR("failed tp load opengl function");
        glfwTerminate();
        return -1;
    }
    glViewport(0,0,WIDTH,HEIGHT);
    glfwSetFramebufferSizeCallback(window,on_window_resize);


    imgui_init(window);


    Shader shader("./shader.vert","./shader.frag");


    uint32_t indices[] = {0 , 1 , 2,};

    Mesh mesh(mesh_vertices,mesh_indices,mesh_vertices.size() / 3);
    
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window,true);
        }

        shader.enable();
        mesh.render();


        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

                ImGui::Button("click me");
                

            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
      

        glfwSwapBuffers(window);
        glClearColor(0.f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    shader.free();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}