#include <stdio.h>
#include <string>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "json.h"


#define WIDTH 800
#define HEIGHT 600
#define ERROR_DY(msg,...) \
    _Pragma("GCC diagnostic push")\
        _Pragma("GCC diagnostic ignored \"-Wformat-extra-args\"")\
    printf("[ERROR] " msg"\n",__VA_ARGS__); \
    _Pragma("GCC diagnostic pop")
#define ERROR(msg) ERROR_DY(msg,NULL) 


std::string load_file_src(const char* file_path) {
    FILE* f = fopen(file_path,"r");
    if(!f) {
        ERROR_DY("failed to open file %s",file_path);
        fclose(f);
    }
    int state;    
    
    state = fseek(f,0,SEEK_END);
    if(state == -1) {
        ERROR_DY("failed to seek the end of the file %s",file_path);
        fclose(f);
        exit(-1);
    }

    long file_size =  ftell(f);
    fseek(f,0,SEEK_SET);

    char* src = (char*)malloc(file_size + 1);
    state = fread(src,1,file_size,f);
    if(state != file_size) {
        ERROR_DY("failed to read the whole file %s",file_path);
        fclose(f);
        exit(-1);
    }
    src[file_size] = '\0';

    std::string file_src(src);

    free(src);
    fclose(f);

    return file_src;
}

static char shader_log[512];
static int shader_status;


void on_window_resize(GLFWwindow* window, int w, int h) {
    // do nothing window is not resizable
}
#include <iostream>
#include <fstream>
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


    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_src = load_file_src("./shader.vert"); 
    const char* c_str_vertex_shader_src = vertex_shader_src.c_str();
    glShaderSource(vertex_shader,1,&c_str_vertex_shader_src,NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader,GL_COMPILE_STATUS,&shader_status);
    if(!shader_status) {
        glGetShaderInfoLog(vertex_shader,512,NULL,shader_log);
        ERROR_DY("failed to compile vertex shader %s",shader_log);
        glfwTerminate();
        return -1;
    }

    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_src = load_file_src("./shader.frag"); 
    const char* c_str_fragment_shader_src = fragment_shader_src.c_str();
    glShaderSource(fragment_shader,1,&c_str_fragment_shader_src,NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader,GL_COMPILE_STATUS,&shader_status);
    if(!shader_status) {
        glGetShaderInfoLog(fragment_shader,512,NULL,shader_log);
        ERROR_DY("failed to compile vertex shader %s",shader_log);
        glfwTerminate();
        return -1;
    }

    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program,vertex_shader);
    glAttachShader(shader_program,fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program,GL_LINK_STATUS,&shader_status);
    if(!shader_status) {
        glGetProgramInfoLog(shader_program,512,NULL,shader_log);
        ERROR_DY("failed to link shader program %s",shader_log);
        glfwTerminate();
        return -1;
    }

    float vertices[] = {
        0.0, 0.5,  0.0,
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


        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,indices);


        glfwSwapBuffers(window);
        glClearColor(0.f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}