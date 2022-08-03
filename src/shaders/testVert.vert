#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 3) in vec3 aColor;

out vec3 pos;
out vec3 normal;
out vec3 color;

uniform mat4 MVP;

void main() {
    pos = aPos;
    normal = aNorm;
    color = aColor;
    
    gl_Position = MVP * vec4(aPos, 1.0);
}