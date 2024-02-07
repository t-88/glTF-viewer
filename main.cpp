#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "utils.hpp"
#include "json.h"

#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define WIDTH 800
#define HEIGHT 600

static char shader_log[512];
static int shader_status;


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
    float* buffer = (float*)(bin_char.data() + buffer_view["byteOffset"].asInt());
    printf("%d\n",position_accessor["count"].asInt());
    for (size_t i = 0; i < position_accessor["count"].asInt(); i++){
        printf("%f %f %f\n",buffer[i * 3] , buffer[i * 3 + 1] , buffer[i * 3 + 2]);
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


    Shader shader("./shader.vert","./shader.frag");

    float vertices[] = {
        0.0, 0.4,  0.0,
        -0.5, -0.5, 0.0,
        0.5,  -0.5,  0.0,
    };

    uint32_t indices[] = {
        0 , 1 , 2,
    };


    uint32_t vao;
    glGenVertexArrays(1,&vao);
    uint32_t vbo; 
    glGenBuffers(1,&vbo);
    uint32_t ebo;
    glGenBuffers(1,&ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER,vbo); // vertex buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float) * 3,0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window,true);
        }

        shader.enable();
        // glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,indices);


        glfwSwapBuffers(window);
        glClearColor(0.f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    shader.free();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}