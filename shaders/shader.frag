#version 330 core
out vec4 FragColor;

in vec3 F_normal;
in vec2 F_tex_coord;



uniform sampler2D base_texture;
uniform sampler2D normal_texture;

uniform vec3 light_dir;
uniform vec3 base_color;

void main() {
    vec3 normal = normalize(F_normal); 
    float intensity =  max(0 , dot(light_dir ,normal)); 

    vec3 ambient = vec3(0.1f);
    vec3 diffuse = vec3(intensity, intensity, intensity);
    vec4 color = vec4(base_color,1.);

    FragColor =  texture(base_texture,vec2(F_tex_coord.x,F_tex_coord.y)) * vec4(diffuse + ambient,1);
}
