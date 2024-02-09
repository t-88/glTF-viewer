#include "pch.hpp"

#include "my_gui.hpp"
#include "utils.hpp"
#include "gltf_loader.hpp"
#include "mesh.hpp"
#include "shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



#define WIDTH 800
#define GUI_WIDTH 200
#define SCENE_WIDTH (WIDTH - GUI_WIDTH)
#define HEIGHT 600

GLFWwindow* window;

void imgui_init(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
void on_window_resize(GLFWwindow* window, int w, int h) {
    glViewport(0,0,w,h);
}
int glfw_init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);


    window = glfwCreateWindow(WIDTH,HEIGHT,"42",NULL,NULL);
    
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
    glViewport(0,0,SCENE_WIDTH,HEIGHT);
    glfwSetFramebufferSizeCallback(window,on_window_resize);

    return 0;
}

int main() {
    if(glfw_init() == -1) { return -1; }
    imgui_init(window);

    GltfLoader gltf_loader("assets/box_indices/box.gltf");
    Shader shader("./shader.vert","./shader.frag");

    Mesh mesh(gltf_loader.vertices,gltf_loader.normals,gltf_loader.indices);

    glm::vec3 rotation(0.);
    glm::mat4x4 rotation_mat(1.);

    glm::vec3 scale(0.25);
    glm::mat4x4 scale_mat = glm::scale(glm::mat4x4(1.),scale);
    
    glm::vec3 translate(0.);
    translate.z = -1;
    glm::mat4x4 translation_mat(1.);
    translation_mat = glm::translate(translation_mat,translate);



    glm::mat4 model = translation_mat * rotation_mat * scale_mat;
    glm::mat4 view = glm::mat4(1.);
    glm::mat4 proj = glm::perspective(glm::radians(45.f),(float)SCENE_WIDTH/HEIGHT,0.1f,100.f);
    
    glm::vec3 light_dir = glm::vec3(0,0,1.);

    shader.enable();
    shader.set_mat4x4("proj",glm::value_ptr(proj));
    shader.set_mat4x4("view",glm::value_ptr(view));
    shader.set_mat4x4("model",glm::value_ptr(model));
    shader.set_vec3("light_dir",std::vector<float>{ light_dir.x,light_dir.y,light_dir.z });


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
            ImGui::SetWindowSize(ImVec2(GUI_WIDTH,HEIGHT));
            ImGui::SetWindowPos(ImVec2(SCENE_WIDTH,0));


                ImGui::Text("Transalte:");
                bool translate_updated = MyGui::InputFloat3WithScroll("##translation",&translate[0],0.1);
                if(translate_updated) {
                    translation_mat = glm::translate(glm::mat4x4(1.),translate);
                }    

                ImGui::Text("Rotation:");
                bool rotation_updated = MyGui::InputFloat3WithScroll("##rotation",&rotation[0],5);
                if(rotation_updated) {
                    rotation_mat = glm::rotate(glm::mat4x4(1.),glm::radians(rotation.x),glm::vec3(1.,0.,0.));
                    rotation_mat = glm::rotate(rotation_mat,glm::radians(rotation.y),glm::vec3(0.,1.,0.));
                    rotation_mat = glm::rotate(rotation_mat,glm::radians(rotation.z),glm::vec3(0.,0.,1.));
                }
                ImGui::Text("Scale:");
                bool scale_updated = MyGui::InputFloat3WithScroll("##scaling",&scale[0]);
                if(scale_updated) {
                    scale_mat = glm::scale(glm::mat4x4(1.),scale);
                }      

                if(translate_updated || rotation_updated || scale_updated) {
                    model = glm::mat4(1.);
                    model = translation_mat * rotation_mat * scale_mat; 
                    shader.enable();
                    shader.set_mat4x4("model",glm::value_ptr(model));
                }          


                

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