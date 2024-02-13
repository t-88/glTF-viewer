#version 330 core

out vec4 FragColor;
in vec3 F_normal;
uniform vec3 light_dir;

uniform vec3 base_color;

void main() {
    // simple diffuse shading    
    vec3 normal = normalize(F_normal); 
    float intensity =  max(0 , dot(light_dir ,normal)); 
    vec3 ambient = vec3(0.1f);
    vec3 diffuse = vec3(intensity, intensity, intensity);


    vec4 color = vec4(base_color,1);

    FragColor =  color * vec4(diffuse + ambient,1);
}
