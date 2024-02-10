#include "pch.hpp"
#include "shader.hpp"
#include "utils.hpp"

static char shader_log[512];
static int shader_status;

Shader::Shader() {}
Shader::Shader(std::string vert_path, std::string frag_path) {
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto vertex_src_str =  load_file_src(vert_path.c_str());
    auto vertex_src_c_str = vertex_src_str.c_str();
    glShaderSource(vertex_shader,1,&vertex_src_c_str,NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader,GL_COMPILE_STATUS,&shader_status);
    if(!shader_status) {
        glGetShaderInfoLog(vertex_shader,512,NULL,shader_log);
        ERROR_DY("failed to compile vertex shader %s",shader_log);
        glfwTerminate();
        exit(-1);
    }

   

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragment_src_str =  load_file_src(frag_path.c_str());
    auto fragment_src_c_str = fragment_src_str.c_str();
    glShaderSource(fragment_shader,1,&fragment_src_c_str,NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader,GL_COMPILE_STATUS,&shader_status);
    if(!shader_status) {
        glGetShaderInfoLog(fragment_shader,512,NULL,shader_log);
        ERROR_DY("failed to compile fragment shader %s",shader_log);
        glfwTerminate();
        exit(-1);
    }
   

    ID = glCreateProgram();
    glAttachShader(ID,vertex_shader);
    glAttachShader(ID,fragment_shader);
    glLinkProgram(ID);
    glGetProgramiv(ID,GL_LINK_STATUS,&shader_status);
    if(!shader_status) {
        glGetProgramInfoLog(ID,512,NULL,shader_log);
        ERROR_DY("failed to link shader program %s",shader_log);
        glfwTerminate();
        exit(-1);
    }

}

Shader::~Shader() {
}

void Shader::free() {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::enable() {
    glUseProgram(ID);
}

void Shader::set_bool(std::string name, bool val) {
    glUniform1i(glGetUniformLocation(ID,name.c_str()),val);
}
void Shader::set_int(std::string name, int val) {
    glUniform1i(glGetUniformLocation(ID,name.c_str()),val);
}
void Shader::set_float(std::string name, float val) {
    glUniform1f(glGetUniformLocation(ID,name.c_str()),val);
}
void Shader::set_vec3(std::string name, std::vector<float> val){ 
    glUniform3f(glGetUniformLocation(ID,name.c_str()),val[0],val[1],val[2]);
}
void Shader::set_mat4x4(std::string name, GLfloat* mat_ptr) {
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,(GLfloat*)mat_ptr);
}



