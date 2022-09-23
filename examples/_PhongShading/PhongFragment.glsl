#version 330 core

out vec4 FragColor;

in vec3 worldPos;
in vec3 normal;
in vec2 uv;

uniform vec3 cameraPos;

// material definition
uniform vec3 albedo;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shiness;

// lights
uniform vec4 ambientColor;

uniform vec3 mainLightDirection;
uniform vec3 mainLightColor;

uniform vec3 pointLightPos;
uniform vec3 pointLightColor;

vec3 DirectionalLight(vec3 direction, vec3 lightColor) {
    vec3 color = albedo;

    vec3 norm = normalize(normal);
    vec3 dir = normalize(-direction);

    float ndotl = max(0, dot(dir, norm));
    vec3 diffuse = ndotl * diffuse.rgb * lightColor.rgb;

    vec3 viewDir = normalize(worldPos - cameraPos);
    vec3 reflectDir = reflect(dir, norm);
    float vdotr = max(dot(viewDir, reflectDir), 0);
    vec3 spec = specular.rgb * pow(vdotr, shiness) * lightColor;

    return spec + color + diffuse;
}

vec3 PointLight(vec3 position, vec3 lightColor) {
    vec3 color = ambientColor.rgb * albedo;

    vec3 norm = normalize(normal);
    vec3 dir = normalize(position - worldPos);

    float ndotl = max(0, dot(dir, norm));
    vec3 diffuse = ndotl * diffuse.rgb * lightColor.rgb;

    vec3 viewDir = normalize(worldPos - cameraPos);
    vec3 reflectDir = reflect(dir, norm);
    float vdotr = max(dot(viewDir, reflectDir), 0);
    vec3 spec = specular.rgb * pow(vdotr, shiness) * lightColor;

    return spec + color + diffuse;
}

void main() {
    vec3 dirLight = DirectionalLight(mainLightDirection, mainLightColor);
    vec3 pointLight = PointLight(pointLightPos, pointLightColor);

    FragColor = vec4(dirLight + pointLight, 1);
}