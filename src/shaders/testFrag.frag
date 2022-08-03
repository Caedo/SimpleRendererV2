#version 330 core

struct DirectionalLight { 
    vec3 direction;
    vec3 color;
};

struct PhongMaterial {
    vec4 albedo;
    vec4 diffuse;
    vec4 specular;
};

uniform PhongMaterial material;
uniform DirectionalLight dirLight;

out vec4 FragColor;

in vec3 pos;
in vec3 normal;
in vec3 color;

void main() {
    vec3 ambient = dirLight.color * 0.1;

    float ndotl = max(dot(normal, normalize(-dirLight.direction)), 0.0);
    vec3 diffuse = dirLight.color * ndotl;

    vec3 color = material.albedo.rgb * (ambient + diffuse);

    FragColor = vec4(color, 1.0);
}