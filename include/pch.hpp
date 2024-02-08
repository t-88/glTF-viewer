#ifndef PCH_HPP
#define PCH_HPP

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

// opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

// imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

// jsoncpp
#include "json.h"

// stb
#include "stb_image.h"


// my classes
#include "utils.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "gltf_loader.hpp"
#include "my_gui.hpp"

#endif