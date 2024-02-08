#version 330 core
out vec4 FragColor;

in vec3 F_normal;

uniform vec3 light_dir;

void main() {
    vec3 normal = normalize(F_normal);
    float intensity =  max(0 , dot(light_dir ,normal)); 


    vec3 ambient = vec3(0.1f);
    vec3 diffuse = vec3(intensity, intensity, intensity);
    vec3 color = vec3(0.6,0.8,0.2);

    FragColor = vec4((diffuse + ambient) * color, 1.0f);
};  
