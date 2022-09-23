#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;

out vec3 worldPos;
out vec3 normal;
out vec2 uv;

uniform mat4 model;
uniform mat4 MVP;

void main() {
    worldPos = (model * vec4(aPos, 1.0)).xyz;
    normal = aNorm;
    uv = aUV;

    gl_Position = MVP * vec4(aPos, 1.0);
}