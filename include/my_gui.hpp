#pragma once

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"


namespace MyGui {
    bool InputFloatWithScroll(const char* label, float* v,float scroll_step);
    bool InputFloat3WithScroll(const char* label, float* v,float scroll_step = 0.1);
}