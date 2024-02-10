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
    stbi_set_flip_vertically_on_load(true);

    GltfLoader gltf_loader("assets/box_textured/BoxTextured.gltf");
    Mesh mesh(gltf_loader);


    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window,true);
        }


        mesh.shader.enable();
        mesh.render();      

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetWindowSize(ImVec2(GUI_WIDTH,HEIGHT));
            ImGui::SetWindowPos(ImVec2(SCENE_WIDTH,0));


                ImGui::Text("Transalte:");
                bool translate_updated = MyGui::InputFloat3WithScroll("##translation",&mesh.translate[0],0.1);
                if(translate_updated) {
                    mesh.translation_mat = glm::translate(glm::mat4x4(1.),mesh.translate);
                }    

                ImGui::Text("Rotation:");
                bool rotation_updated = MyGui::InputFloat3WithScroll("##rotation",&mesh.rotation[0],5);
                if(rotation_updated) {
                    mesh.rotation_mat = glm::rotate(glm::mat4x4(1.),glm::radians(mesh.rotation.x),glm::vec3(1.,0.,0.));
                    mesh.rotation_mat = glm::rotate(mesh.rotation_mat,glm::radians(mesh.rotation.y),glm::vec3(0.,1.,0.));
                    mesh.rotation_mat = glm::rotate(mesh.rotation_mat,glm::radians(mesh.rotation.z),glm::vec3(0.,0.,1.));
                }
                ImGui::Text("Scale:");
                bool scale_updated = MyGui::InputFloat3WithScroll("##scaling",&mesh.scale[0]);
                if(scale_updated) {
                    mesh.scale_mat = glm::scale(glm::mat4x4(1.),mesh.scale);
                }      

                if(translate_updated || rotation_updated || scale_updated) {
                    auto model = glm::mat4(1.);
                    model = mesh.translation_mat * mesh.rotation_mat * mesh.scale_mat; 
                    mesh.shader.enable();
                    mesh.shader.set_mat4x4("model",glm::value_ptr(model));
                }          


                

            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
      


        glfwSwapBuffers(window);
        glClearColor(0.f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    

    mesh.shader.free();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}