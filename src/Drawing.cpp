#include "SimpleRenderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

Shader ErrorShader;
Shader ColorShader;
Shader TextureShader;
Shader VertexColorShader;
Shader ScreenSpaceShader;

//=========================================
// Default shaders
//=========================================

const char* DefaultVertexShaderSource = 
R"###(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec4 aColor;
out vec3 pos;
out vec3 normal;
out vec2 uv;
out vec4 vertexColor;
uniform mat4 MVP;
void main() {
    pos = aPos;
    normal = aNorm;
    uv = aUV;
    vertexColor = aColor;

    gl_Position = MVP * vec4(aPos, 1.0);
})###";

const char* VertexColorShaderSource =
R"###(#version 330 core
uniform vec4 tint;
in vec4 vertexColor;
out vec4 FragColor;
void main() {
    FragColor = vertexColor;
})###";

const char* ColorShaderSource =
R"###(#version 330 core
uniform vec4 tint;
out vec4 FragColor;
void main() {
    FragColor = tint;
})###";

const char* TextureShaderSource =
R"###(#version 330 core
uniform vec4 tint;
uniform sampler2D tex;
in vec2 uv;
out vec4 FragColor;
void main() {
    vec4 color = texture(tex, uv);
    FragColor = color;
})###";

const char* ErrorFragmentShaderSource =
R"###(#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1, 0, 1, 1);
})###";


const char* ScreenSpaceVertexSource = 
R"###(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 aColor;

uniform int framebufferWidth;
uniform int framebufferHeight;

out vec2 uv;
out vec4 vertexColor;

vec3 ScreenToClip(vec3 pos) {
    vec2 p = pos.xy / vec2(framebufferWidth, framebufferHeight);
    p = p * 2 - 1;
    p.y *= -1;

    return vec3(p.x, p.y, 0);
}

void main() {
    uv = aUV;
    vertexColor = aColor;

    gl_Position = vec4(ScreenToClip(aPos), 1);
})###";


const char* ScreenSpaceFragmentSource = 
R"###(#version 330 core
in vec2 uv;
in vec4 vertexColor;

uniform sampler2D tex;

out vec4 FragColor;

void main() {
    vec4 col = texture(tex, uv);
    FragColor = vertexColor * col;
})###";


//=========================================
// GL state
//=========================================

void FaceCulling(SRWindow* window, bool enabled) {
    window->faceCulling = enabled;
    if(enabled) {
        glEnable(GL_CULL_FACE);

        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
}

void DepthTest(SRWindow* window, bool enabled) {
    window->depthTest = enabled;

    if(enabled) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}

void UseShader(SRWindow* window, Shader shader) {
    uint32_t id = shader.isValid ? shader.id : ErrorShader.id;

    window->currentShaderId = id;
    glUseProgram(id);
}

void ClearColorBuffer(Vector4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ClearDepthBuffer() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ClearColorAndDepthBuffer(Vector4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//=========================================
// Shaders
//=========================================

Shader LoadShaderSource(const char *vertexShaderCode, const char *fragmentShaderCode)
{
    Shader ret = {0};

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "[Error] Vertex Shader compilation failed: \n%s\n", infoLog);

        return ret;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "[Error] Fragment shader compilation failed: \n%s\n", infoLog);

        return ret;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);

        return ret;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    ret.id = shaderProgram;

    ret.isValid = true;
    return ret;
}

Shader LoadShaderFromFile(const char *vertexPath, const char *fragmentPath, MemoryArena* arena)
{
    char *vertexSource = NULL;
    char *fragmentSource = NULL;

    FILE *vertexFile;
    errno_t err = fopen_s(&vertexFile, vertexPath, "rb");
    if(err == 0)
    {
        // TODO: ReadEntireFile
        fseek(vertexFile, 0, SEEK_END);
        long length = ftell(vertexFile);
        fseek(vertexFile, 0, SEEK_SET);

        vertexSource = (char *)PushArena(arena, length + 1);
        fread(vertexSource, 1, length, vertexFile);

        vertexSource[length] = '\0';
        fclose(vertexFile);
    }
    else
    {
        printf("[ERROR] Failed to open vertex shader source file at path: %s\n", vertexPath);
        return {0};
    }

    FILE *fragmentFile;
    err = fopen_s(&fragmentFile, fragmentPath, "rb");
    if(err == 0)
    {
        // TODO: ReadEntireFile

        fseek(fragmentFile, 0, SEEK_END);
        long length = ftell(fragmentFile);
        fseek(fragmentFile, 0, SEEK_SET);

        fragmentSource = (char *)PushArena(arena, length + 1);
        fread(fragmentSource, 1, length, fragmentFile);

        fragmentSource[length] = '\0';
        fclose(fragmentFile);
    }
    else
    {
        printf("[ERROR] Failed to open fragment shader source file at path: %s\n", fragmentPath);
        return {0};
    }

    return LoadShaderSource(vertexSource, fragmentSource);
}

void SetUniformInt(Shader shader, char *name, int value) {
    glUseProgram(shader.id);
    int loc = glGetUniformLocation(shader.id, name);
    if (loc != -1)
    {
        glUniform1i(loc, value);
    }
    else {
        // @TODO: logger
        // printf("Couldn't find uniform Float: %s\n", name);
    }
}

void SetUniformFloat(Shader shader, char *name, float value) {
    glUseProgram(shader.id);
    int loc = glGetUniformLocation(shader.id, name);
    if (loc != -1)
    {
        glUniform1f(loc, value);
    }
    else {
        // @TODO: logger
        // printf("Couldn't find uniform Float: %s\n", name);
    }
}

void SetUniformVec2(Shader shader, char *name, Vector2 value) {
    glUseProgram(shader.id);
    int loc = glGetUniformLocation(shader.id, name);
    if(loc != -1) {
        glUniform2f(loc, value.x, value.y);
    }
    else {
        // @TODO: logger
        // printf("Couldn't find uniform Vec2: %s\n", name);
    }
}

void SetUniformVec3(Shader shader, char *name, Vector3 value){
    glUseProgram(shader.id);
    int loc = glGetUniformLocation(shader.id, name);
    if (loc != -1) {
        glUniform3f(loc, value.x, value.y, value.z);
    }
    else {
        // @TODO: logger
        // printf("Couldn't find uniform Vec3: %s\n", name);
    }
}

void SetUniformColor(Shader shader, char *name, Vector4 value) {
    glUseProgram(shader.id);
    int loc = glGetUniformLocation(shader.id, name);
    if (loc != -1) {
        // glUniform4f(loc, value.x, value.y, value.z, value.w);
        glUniform4fv(loc, 1, (const float *)(&value));
    }
    else {
        // @TODO: logger
        // printf("Couldn't find uniform color: %s\n", name);
    }
}

void SetUniformMatrix(Shader shader, char* name, Matrix value) {
    glUseProgram(shader.id);
    int loc = glGetUniformLocation(shader.id, name);
    if (loc != -1) {
        // glUniform4f(loc, value.x, value.y, value.z, value.w);
        glUniformMatrix4fv(loc, 1, false, (const float *)(&value));
    }
    else {
        // @TODO: logger
        // printf("Couldn't find uniform color: %s\n", name);
    }
}

//=========================================
// Meshes
//=========================================

void ApplyMesh(Mesh *mesh)
{
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->positionsVBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->positionsVBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.length * sizeof(Vector3), mesh->vertices.data, GL_STATIC_DRAW);

    glVertexAttribPointer(VertexPositionIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(VertexPositionIndex);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->triangles.length * sizeof(int), mesh->triangles.data, GL_STATIC_DRAW);

    if (mesh->normals.length != 0)
    {
        glGenBuffers(1, &mesh->normalsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->normalsVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->normals.length * sizeof(Vector3), mesh->normals.data, GL_STATIC_DRAW);

        glVertexAttribPointer(VertexNormalIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(VertexNormalIndex);
    }

    if (mesh->uv.length != 0)
    {
        glGenBuffers(1, &mesh->uvVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->uvVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->uv.length * sizeof(Vector2), mesh->uv.data, GL_STATIC_DRAW);

        glVertexAttribPointer(VertexUVIndex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(VertexUVIndex);
    }

    if (mesh->colors.length != 0)
    {
        glGenBuffers(1, &mesh->colorsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->colorsVBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->colors.length * sizeof(Vector4), mesh->colors.data, GL_STATIC_DRAW);

        glVertexAttribPointer(VertexColorIndex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(VertexColorIndex);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DeleteMesh(Mesh* mesh) {
    assert(mesh);

    glDeleteVertexArrays(1, &mesh->VAO);
    glDeleteBuffers(1, &mesh->EBO);
    glDeleteBuffers(1, &mesh->positionsVBO);
    glDeleteBuffers(1, &mesh->normalsVBO);
    glDeleteBuffers(1, &mesh->colorsVBO);
    glDeleteBuffers(1, &mesh->uvVBO);
}

Mesh CreateQuadMesh()
{
    Mesh mesh = {};
    mesh.vertices = AllocateSlice<Vector3>(4);
    mesh.vertices[0] = {0.5f, 0.5f, 0.0f},   // top right
    mesh.vertices[1] = {0.5f, -0.5f, 0.0f},   // top right
    mesh.vertices[2] = {-0.5f, -0.5f, 0.0f},   // top right
    mesh.vertices[3] = {-0.5f, 0.5f, 0.0f},   // top right

    mesh.triangles = AllocateSlice<int>(6);
    mesh.triangles[0] = 0;
    mesh.triangles[1] = 3;
    mesh.triangles[2] = 1;

    mesh.triangles[3] = 1;
    mesh.triangles[4] = 3;
    mesh.triangles[5] = 2;

    mesh.normals = AllocateSlice<Vector3>(4);
    mesh.normals[0] = {0, 0, 1};
    mesh.normals[1] = {0, 0, 1};
    mesh.normals[2] = {0, 0, 1};
    mesh.normals[3] = {0, 0, 1};

    ApplyMesh(&mesh);

    return mesh;
}

Mesh CreateCubeMesh()
{
    Mesh mesh = {};

    mesh.vertices  = AllocateSlice<Vector3>(24);
    mesh.normals   = AllocateSlice<Vector3>(24);
    mesh.uv        = AllocateSlice<Vector2>(24);
    mesh.colors    = AllocateSlice<Vector4>(24);
    mesh.triangles = AllocateSlice<int>(36);

    // front
    mesh.vertices[0] = {0.5f, 0.5f, 0.5f},   // top right 0
    mesh.vertices[1] = {0.5f, -0.5f, 0.5f},  // bottom right 1
    mesh.vertices[2] = {-0.5f, -0.5f, 0.5f}, // bottom left 2
    mesh.vertices[3] = {-0.5f, 0.5f, 0.5f},  // top left 3

    mesh.colors[0] = {0, 0, 1, 1};
    mesh.colors[1] = {0, 1, 0, 1};
    mesh.colors[2] = {0, 1, 1, 1};
    mesh.colors[3] = {1, 0, 0, 1};

    mesh.uv[0] = {1, 1};
    mesh.uv[1] = {1, 0};
    mesh.uv[2] = {0, 0};
    mesh.uv[3] = {0, 1};

    // back
    mesh.vertices[4] = {0.5f, 0.5f, -0.5f},   // top right 4
    mesh.vertices[5] = {0.5f, -0.5f, -0.5f},  // bottom right 5
    mesh.vertices[6] = {-0.5f, -0.5f, -0.5f}, // bottom left 6
    mesh.vertices[7] = {-0.5f, 0.5f, -0.5f},  // top left 7

    mesh.colors[4] = {1, 0, 1, 1};
    mesh.colors[5] = {1, 1, 0, 1};
    mesh.colors[6] = {1, 1, 1, 1};
    mesh.colors[7] = {0, 0, 0, 1};

    mesh.uv[4] = {1, 1};
    mesh.uv[5] = {1, 0};
    mesh.uv[6] = {0, 0};
    mesh.uv[7] = {0, 1};

    // left
    mesh.vertices[8] = {-0.5f, 0.5f, 0.5f},   // top front 8
    mesh.vertices[9] = {-0.5f, -0.5f, 0.5f},  // bottom front 9
    mesh.vertices[10] = {-0.5f, -0.5f, -0.5f}, // bottom back 10
    mesh.vertices[11] = {-0.5f, 0.5f, -0.5f},  // top back 11

    mesh.colors[8] =  {1, 0, 0, 1};
    mesh.colors[9] =  {0, 1, 1, 1};
    mesh.colors[10] = {1, 1, 1, 1};
    mesh.colors[11] = {0, 0, 0, 1};

    mesh.uv[8] = {1, 1};
    mesh.uv[9] = {1, 0};
    mesh.uv[10] = {0, 0};
    mesh.uv[11] = {0, 1};

    // right
    mesh.vertices[12] = {0.5f, 0.5f, 0.5f},   // top front 12
    mesh.vertices[13] = {0.5f, -0.5f, 0.5f},  // bottom front 13
    mesh.vertices[14] = {0.5f, -0.5f, -0.5f}, // bottom back 14
    mesh.vertices[15] = {0.5f, 0.5f, -0.5f},  // top back 15

    mesh.colors[12] = {0, 0, 1, 1};
    mesh.colors[13] = {0, 1, 0, 1};
    mesh.colors[14] = {1, 1, 0, 1};
    mesh.colors[15] = {1, 0, 1, 1};

    mesh.uv[12] = {1, 1};
    mesh.uv[13] = {1, 0};
    mesh.uv[14] = {0, 0};
    mesh.uv[15] = {0, 1};

    // top
    mesh.vertices[16] = {-0.5f, 0.5f, 0.5f},  // left front 16
    mesh.vertices[17] = {0.5f, 0.5f, 0.5f},   // right front 17
    mesh.vertices[18] = {0.5f, 0.5f, -0.5f},  // right back 18
    mesh.vertices[19] = {-0.5f, 0.5f, -0.5f}, // left back 19

    mesh.colors[16] = {1, 0, 0, 1};
    mesh.colors[17] = {0, 0, 1, 1};
    mesh.colors[18] = {1, 0, 1, 1};
    mesh.colors[19] = {0, 0, 0, 1};

    mesh.uv[16] = {1, 1};
    mesh.uv[17] = {1, 0};
    mesh.uv[18] = {0, 0};
    mesh.uv[19] = {0, 1};

    // bottom
    mesh.vertices[20] = {-0.5f, -0.5f, 0.5f},  // left front 20
    mesh.vertices[21] = {0.5f, -0.5f, 0.5f},   // right front 21
    mesh.vertices[22] = {0.5f, -0.5f, -0.5f},  // right back 22
    mesh.vertices[23] = {-0.5f, -0.5f, -0.5f}, // left back 23

    mesh.colors[20] = {0, 1, 1, 1};
    mesh.colors[21] = {0, 1, 0, 1};
    mesh.colors[22] = {1, 1, 0, 1};
    mesh.colors[23] = {1, 1, 1, 1};

    mesh.uv[20] = {1, 1};
    mesh.uv[21] = {1, 0};
    mesh.uv[22] = {0, 0};
    mesh.uv[23] = {0, 1};

    // {
    //     0, 2, 1,    //
    //     0, 3, 2,    // front
    //     4, 5, 6,    //
    //     4, 6, 7,    // back
    //     8, 10, 9,   //
    //     8, 11, 10,  // left
    //     12, 13, 14, //
    //     12, 14, 15, // right
    //     16, 17, 18, //
    //     16, 18, 19, // top
    //     20, 23, 22, //
    //     20, 22, 21  // bottom
    // };

    mesh.triangles[0] = 0; mesh.triangles[1] = 2; mesh.triangles[2] = 1;
    mesh.triangles[3] = 0; mesh.triangles[4] = 3; mesh.triangles[5] = 2; // front

    mesh.triangles[6] = 4; mesh.triangles[7]  = 5;  mesh.triangles[8] = 6;
    mesh.triangles[9] = 4; mesh.triangles[10] = 6; mesh.triangles[11] = 7; // back

    mesh.triangles[12] = 8; mesh.triangles[13] = 10; mesh.triangles[14] = 9;
    mesh.triangles[15] = 8; mesh.triangles[16] = 11; mesh.triangles[17] = 10; // left
    
    mesh.triangles[18] = 12; mesh.triangles[19] = 13; mesh.triangles[20] = 14;
    mesh.triangles[21] = 12; mesh.triangles[22] = 14; mesh.triangles[23] = 15; // right

    mesh.triangles[24] = 16; mesh.triangles[25] = 17; mesh.triangles[26] = 18;
    mesh.triangles[27] = 16; mesh.triangles[28] = 18; mesh.triangles[29] = 19; // top

    mesh.triangles[30] = 20; mesh.triangles[31] = 23; mesh.triangles[32] = 22;
    mesh.triangles[33] = 20; mesh.triangles[34] = 22; mesh.triangles[35] = 21; // bottm

    CalculateNormals(&mesh);
    ApplyMesh(&mesh);

    return mesh;
}

Mesh CreatePlaneMesh() {
    Mesh mesh = {};

    const int meshSize = 11;

    int vertsCount     = meshSize * meshSize;
    int trianglesCount = (meshSize - 1) * (meshSize - 1) * 6;

    mesh.vertices  = AllocateSlice<Vector3>(vertsCount);
    mesh.normals   = AllocateSlice<Vector3>(vertsCount);
    mesh.triangles = AllocateSlice<int>(trianglesCount);

    float offset = meshSize / 2.0f - 0.5f;

    for(int y = 0; y < meshSize; y++) {
        for(int x = 0; x < meshSize; x++) {
            int idx = x + y * meshSize;
            mesh.vertices[idx] = {(float) x - offset, 0, (float) y - offset};
            mesh.normals[idx]  = {0, 1, 0};
        }
    }

    int index = 0;
    for (int y = 0; y < meshSize - 1; y++) {
        for (int x = 0; x < meshSize - 1; x++) {
            int idx = x + y * meshSize;

            mesh.triangles[index++] = idx;
            mesh.triangles[index++] = idx + meshSize + 1;
            mesh.triangles[index++] = idx + 1;

            mesh.triangles[index++] = idx;
            mesh.triangles[index++] = idx + meshSize;
            mesh.triangles[index++] = idx + meshSize + 1;
        }
    }

    ApplyMesh(&mesh);
    return mesh;
}


Mesh CreateUVSphereMesh()
{
    Mesh mesh = {};

    const int meshSize = 16;

    int vertsCount     = (meshSize - 1) * meshSize + 2;
    int trianglesCount = (meshSize - 1) * (meshSize - 1) * 6 + 2 * ((meshSize - 1) * 3);

    mesh.vertices  = AllocateSlice<Vector3>(vertsCount);
    mesh.normals   = AllocateSlice<Vector3>(vertsCount);
    mesh.triangles = AllocateSlice<int>(trianglesCount);

    int index = 0;
    mesh.vertices[index++] = {0, 1, 0};
    for(int i = 0; i < meshSize - 1; i++) {
        double p = PI * (double) (i + 1) / meshSize;
        double pSin = sin(p);
        double pCos = cos(p);

        for(int j = 0; j < meshSize; j++) {
            double a = 2.0 * PI * (double) j / meshSize;
            double aSin = sin(a);
            double aCos = cos(a);

            mesh.vertices[index++] = { (float) (pSin * aCos), (float) pCos, (float) (pSin * aSin) };
        }
    }
    mesh.vertices[index++] = {0, -1, 0};

    index = 0;
    for(int i = 0; i < meshSize; i++) {
        mesh.triangles[index++] = 0;
        mesh.triangles[index++] = ((i + 1) % meshSize) + 1;
        mesh.triangles[index++] = i + 1;
    }

    for (int y = 0; y < meshSize - 2; y++) {
        for (int x = 0; x < meshSize; x++) {
            int idx = x + y * meshSize + 1;

            int t1 = idx;
            int t2 = (x == meshSize - 1) ? idx - meshSize + 1 : idx + 1;
            int t3 = idx + meshSize;
            int t4 = (x == meshSize - 1) ? idx + 1 : idx + meshSize + 1;

            mesh.triangles[index++] = t1;
            mesh.triangles[index++] = t2;
            mesh.triangles[index++] = t4;

            mesh.triangles[index++] = t1;
            mesh.triangles[index++] = t4;
            mesh.triangles[index++] = t3;
        }
    }

    for(int i = 0; i < meshSize; i++) {
        mesh.triangles[index++] = vertsCount - 1;
        mesh.triangles[index++] = (i == meshSize - 1) ? vertsCount - 2 : vertsCount - 3 - i;
        mesh.triangles[index++] = vertsCount - 2 - i;
    }

    CalculateNormals(&mesh);
    ApplyMesh(&mesh);

    return mesh;
}


void CalculateNormals(Mesh *mesh) {
    assert(mesh && mesh->vertices.data && mesh->triangles.data && mesh->normals.data);

    Slice<Vector3> verts = mesh->vertices;
    Slice<int32_t> tris = mesh->triangles;

    memset(mesh->normals.data, 0, mesh->normals.length * sizeof(Vector3));

    for (int i = 0; i < mesh->triangles.length; i += 3)
    {
        int indexA = tris[i];
        int indexB = tris[i + 1];
        int indexC = tris[i + 2];

        Vector3 a = verts[indexA];
        Vector3 b = verts[indexB];
        Vector3 c = verts[indexC];

        Vector3 ab = a - b;
        Vector3 ac = a - c;

        Vector3 normal = Vector3CrossProduct(ab, ac);

        mesh->normals[indexA] = mesh->normals[indexA] + normal;
        mesh->normals[indexB] = mesh->normals[indexB] + normal;
        mesh->normals[indexC] = mesh->normals[indexC] + normal;
    }

    for (int i = 0; i < mesh->vertices.length; i++) {
        mesh->normals[i] = Vector3Normalize(mesh->normals[i]);
    }
}

//========================================
// Textures
//========================================

Texture LoadTextureAtPath(char* path, MemoryArena* arena) {
    assert(path);

    Texture ret = {};

    FILE* file;
    errno_t err = fopen_s(&file, path, "rb");
    if(err != 0) {
        return ret;
    }

    // TODO: ReadEntireFile
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* fileData = (char *)PushArena(arena, length);
    fread(fileData, 1, length, file);
    fclose(file);

    return LoadTextureFromMemory(MakeSlice(fileData, 0, length));
}

Texture LoadTextureFromMemory(Slice<char> memory) {
    Texture ret = {};

    unsigned char* texData = stbi_load_from_memory(
        (const stbi_uc*) memory.data,
        (int) memory.length,
        &ret.width,
        &ret.height,
        &ret.channels,
        0
    );

    GLenum format = 0;
    switch(ret.channels) {
        case 1: format = GL_RED;  break;
        case 2: format = GL_RG;   break;
        case 3: format = GL_RGB;  break;
        case 4: format = GL_RGBA; break;
        default: assert(false);
    }

    glGenTextures(1, &ret.id);
    glBindTexture(GL_TEXTURE_2D, ret.id);

    // TODO: Handle different parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, ret.width, ret.height, 0, format, GL_UNSIGNED_BYTE, texData);

    stbi_image_free(texData);

    glBindTexture(GL_TEXTURE_2D, 0);
    ret.isValid = true;

    return ret;
}

void BindTexture(Texture texture) {
    // @TODO: add texture units
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

//========================================
// Drawing
//========================================

// void PrintMatrix(Matrix mat) {
//     for(int i = 0; i < 4; i++) {
//         printf("{%f, %f, %f, %f}\n", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
//     }
//     printf("\n");
// }

void DrawMesh(SRWindow* window, Mesh mesh, Matrix transform) {
    uint32_t mvpLoc = glGetUniformLocation(window->currentShaderId, "MVP");
    if(mvpLoc != -1)
        glUniformMatrix4fv(mvpLoc, 1, false, (const float *)(&transform));

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.triangles.length, GL_UNSIGNED_INT, 0);
}

void DrawMesh(SRWindow* window, Mesh mesh, Camera camera, Matrix transform) {
    Matrix projection = GetProjection(&camera);
    Matrix view = GetView(&camera);
    Matrix mvp = projection * view * transform;

    uint32_t mvpLoc = glGetUniformLocation(window->currentShaderId, "MVP");
    if(mvpLoc != -1)
        glUniformMatrix4fv(mvpLoc, 1, false, (const float *)(&mvp));

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.triangles.length, GL_UNSIGNED_INT, 0);
}

//========================================
// Batch
//========================================
void RenderBatch(SRWindow* window, Batch* batch) {
    if(batch->currentSize == 0) {
        return;
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(ScreenSpaceShader.id);

    glBindTexture(GL_TEXTURE_2D, batch->usedTextureId);

    glBindVertexArray(batch->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BatchVertex) * batch->currentSize, batch->vertices);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) batch->currentSize);

    batch->currentSize = 0;
    batch->usedTextureId = batch->whiteTextureId;

    // Restore previous GL State
    if(window->faceCulling) {
        glEnable(GL_CULL_FACE);
    }

    if(window->depthTest) {
        glEnable(GL_DEPTH_TEST);
    }

    glUseProgram(window->currentShaderId);
    glBindTexture(GL_TEXTURE_2D, 0);
}

//========================================
// Screen Space drawing
//========================================
void DrawRect(SRWindow* window, Rect rect, Vector4 color) {
    assert(window->state == ScreenSpace);

    Batch* batch = &window->screenSpaceBatch;
    if(batch->usedTextureId != batch->whiteTextureId) {
        RenderBatch(window, &window->screenSpaceBatch);
    }

    batch->usedTextureId = batch->whiteTextureId;

    float left  = rect.x;
    float right = rect.x + rect.width;
    float top   = rect.y;
    float bot   = rect.y + rect.height;

    BatchVertex vertices[6];
    vertices[0] = {{left,  top, 0}, {0, 0}, color};
    vertices[1] = {{right, top, 0}, {1, 0}, color};
    vertices[2] = {{right, bot, 0}, {1, 1}, color};

    vertices[3] = {{left,  top, 0}, {0, 0}, color};
    vertices[4] = {{right, bot, 0}, {1, 1}, color};
    vertices[5] = {{left,  bot, 0}, {0, 1}, color};

    AddBatchVertices(window, &window->screenSpaceBatch, MakeSlice(vertices, 0, 6));
}

void DrawTexture(SRWindow* window, Texture texture, Vector2 position, Vector2 origin) {
    assert(window->state == ScreenSpace);

    if(window->screenSpaceBatch.usedTextureId != texture.id) {
        RenderBatch(window, &window->screenSpaceBatch);
    }

    window->screenSpaceBatch.usedTextureId = texture.id;

    float left  = position.x - texture.width * origin.x;
    float right = position.x + texture.width * (1 - origin.x);
    float top   = position.y - texture.height * origin.y;
    float bot   = position.y + texture.height * (1 - origin.y);

    BatchVertex vertices[6];
    vertices[0] = {{left,  top, 0}, {0, 0}, {1, 1, 1, 1}};
    vertices[1] = {{right, top, 0}, {1, 0}, {1, 1, 1, 1}};
    vertices[2] = {{right, bot, 0}, {1, 1}, {1, 1, 1, 1}};

    vertices[3] = {{left,  top, 0}, {0, 0}, {1, 1, 1, 1}};
    vertices[4] = {{right, bot, 0}, {1, 1}, {1, 1, 1, 1}};
    vertices[5] = {{left,  bot, 0}, {0, 1}, {1, 1, 1, 1}};

    AddBatchVertices(window, &window->screenSpaceBatch, MakeSlice(vertices, 0, 6));
}

void DrawTextureFragment(SRWindow* window, Texture texture, Rect source, Rect destination){
    assert(window->state == ScreenSpace);

    if(window->screenSpaceBatch.usedTextureId != texture.id) {
        RenderBatch(window, &window->screenSpaceBatch);
    }

    window->screenSpaceBatch.usedTextureId = texture.id;

    float left  = destination.x;
    float right = destination.x + destination.width;
    float top   = destination.y;
    float bot   = destination.y + destination.height;

    float uvLeft  = source.x;
    float uvRight = source.x + source.width;
    float uvTop   = source.y;
    float uvBot   = source.y + source.height;


    BatchVertex vertices[6];
    vertices[0] = {{left,  top, 0}, {uvLeft,  uvTop}, {1, 1, 1, 1}};
    vertices[1] = {{right, top, 0}, {uvRight, uvTop}, {1, 1, 1, 1}};
    vertices[2] = {{right, bot, 0}, {uvRight, uvBot}, {1, 1, 1, 1}};

    vertices[3] = {{left,  top, 0}, {uvLeft,  uvTop}, {1, 1, 1, 1}};
    vertices[4] = {{right, bot, 0}, {uvRight, uvBot}, {1, 1, 1, 1}};
    vertices[5] = {{left,  bot, 0}, {uvLeft,  uvBot}, {1, 1, 1, 1}};

    AddBatchVertices(window, &window->screenSpaceBatch, MakeSlice(vertices, 0, 6));
}

