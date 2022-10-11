#version 330 core

uniform vec2 resolution;
uniform vec3 cameraPos;

// uniform mat4 viewMatrix;
uniform vec3 viewTarget;
uniform vec3 viewUp;

uniform float iTime;

uniform vec3 lightDir;
uniform vec3 albedo;
uniform vec3 lightColor;

in vec4 clipSpacePos;

out vec4 FragColor;

#define EPSILON 0.001

float sphere(vec3 p, float r) {
    return length(p) - r;
}

float opSmoothUnion(float d1, float d2, float k) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float sceneSDF(vec3 point) {
    float t = iTime * 0.7;
    vec3 s1Pos = vec3(0.0, sin(t)         + 4, 0.0);
    vec3 s2Pos = vec3(0.5, sin(t - 60.0)  + 4 , 0.0);
    vec3 s3Pos = vec3(-0.2, sin(t - 90.0) + 4, 0.0);

    float s1 = sphere(point - s1Pos, 0.6);
    float s2 = sphere(point - s2Pos, 0.5);
    float s3 = sphere(point - s3Pos, 0.45);

    float sd = opSmoothUnion(s1, s2, 0.2);
    sd = opSmoothUnion(sd, s3, 0.25);

    return sd;
}

mat4 ViewMatrix(vec3 pos, vec3 target, vec3 up) {
    vec3 f = normalize(target - pos);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(-s, 0.0),
        vec4(-u, 0.0),
        vec4(f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}

float rayMarch(vec3 rayOrigin, vec3 rayDirection, float start, float end) {
    float depth = start;
    for(int i = 0; i < 255; i++) {
        float dist = sceneSDF(rayOrigin + depth * rayDirection);
        
        if(dist < EPSILON) return depth;
        
        depth += dist;
        if(depth >= end) return end;
    }
    
    return end;
}

vec3 normal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

vec3 DirectionalLight(vec3 worldPos, vec3 normal, vec3 lightDir) {
    vec3 color = albedo;

    float ndotl = max(0, dot(lightDir, normal));
    vec3 diffuse = ndotl * lightColor * 2;

    vec3 viewDir = normalize(worldPos - cameraPos);
    vec3 reflectDir = reflect(lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
    vec3 specular = 0.3 * spec * lightColor;

    return (specular + diffuse + vec3(0.3, 0.3, 0.3)) * color;
}


void main() {
    vec3 p = clipSpacePos.xyz / clipSpacePos.w;
    p = p * 0.5 + 0.5;

    vec2 fragCoord = p.xy * resolution;
    vec3 dir = -rayDirection(60, resolution, fragCoord);

    mat4 cam = ViewMatrix(cameraPos, viewTarget, viewUp);
    dir = (cam * vec4(dir, 0.0)).xyz;

    float dist = rayMarch(cameraPos, dir, 0.0, 100.0);

    if(dist > 100.0 - EPSILON) {
        FragColor = vec4(p.xy, 0.0, 0.0);
        return;
    }

    vec3 hitPoint = cameraPos + dist * dir;
    vec3 normal = normal(hitPoint);

    vec3 col = DirectionalLight(hitPoint, normal, lightDir);

    FragColor = vec4(col, 1);
}