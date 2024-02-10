#version 330 core
out vec4 FragColor;

void main() {
    vec3 color = vec3(0.6,0.8,0.2);
    FragColor = vec4(color, 1.0f);
};  
