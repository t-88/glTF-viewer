#include <string>
#include "my_gui.hpp"


    bool MyGui::InputFloatWithScroll(const char* label, float* v,float scroll_step) {
    bool out = ImGui::InputFloat(label,v);
        if(!ImGui::IsItemHovered()) return out;

        float wheel = ImGui::GetIO().MouseWheel;
        if(wheel) {
            *v += wheel  * scroll_step;
            return true;
        } 
        return out;
    }

bool MyGui::InputFloat3WithScroll(const char* label, float* v,float scroll_step) {
    ImGui::PushItemWidth(60);
        bool out = InputFloatWithScroll((std::string(label) + std::string("0")).c_str(),&v[0],scroll_step);
        ImGui::SameLine();
        ImGui::NextColumn();
        out = InputFloatWithScroll((std::string(label) + std::string("1")).c_str(),&v[1],scroll_step) || out;
        ImGui::SameLine();
        out = InputFloatWithScroll((std::string(label) + std::string("2")).c_str(),&v[2],scroll_step) || out;
    ImGui::PopItemWidth();

    return out;
}


