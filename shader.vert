#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform vec3 light_dir;
out vec3 F_normal;
void main() {
   vec4 pos = vec4(aPos,1.);
   pos = proj * view * model * pos ; 
   pos /= pos.w;  

   F_normal = normalize(mat3(transpose(inverse(model))) * aNormal);  
   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
};
