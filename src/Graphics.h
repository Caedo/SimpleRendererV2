#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "ctype.h"

#include "Camera.h"

struct GLFWwindow;

enum VertexBufferIndex {
    VertexPositionIndex = 0,
    VertexNormalIndex   = 1,
    VertexUVIndex       = 2,
    VertexColorIndex    = 3,
};

struct Shader
{
    bool isValid;
    uint32_t id;
};

struct Mesh
{
    int vertsCount;
    int trianglesCount;

    Vector3* vertices;
    Vector3* normals;
    Vector3* colors;
    Vector2* uv;
    uint32_t* triangles;

    uint32_t VAO;
    uint32_t EBO;

    uint32_t positionsVBO;
    uint32_t normalsVBO;
    uint32_t colorsVBO;
    uint32_t uvVBO;
};

struct PhongMaterial
{
    Shader shader;

    Vector4 albedo;
    Vector4 diffuse;
    Vector4 specular;
};

struct DirectionalLight 
{
    Vector3 direction;
    Vector3 color;
};

//=========================================
// Default shaders
//=========================================

const char* DefaultVertexShader = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNorm;\n"
"layout (location = 3) in vec4 aColor;\n"
"out vec3 pos;\n"
"out vec3 normal;\n"
"out vec4 color;\n"
"uniform mat4 MVP;\n"
"void main() {\n"
"    pos = aPos;\n"
"    normal = aNorm;\n"
"    color = aColor;\n"
"\n"
"    gl_Position = MVP * vec4(aPos, 1.0);\n"
"}";

const char* DefaultFragmentShader =
"#version 330 core\n"
"uniform vec4 tint;\n"
"in vec3 pos;\n"
"in vec3 normal;\n"
"in vec4 vertexColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = tint;\n"
"}";

const char* ErrorFragmentShader =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1, 0, 1, 1);\n"
"}";

//=========================================
// Initialization
//=========================================

GLFWwindow* CreateGLFWWindow(int width, int height, char* tittle);
void InitalizeRenderer();

//=========================================
// Shaders
//=========================================

Shader LoadShaderSource(const char *vertexShader, const char *fragmentShader);
Shader LoadShaderFromFile(const char *vertexPath, const char *fragmentPath);

void SetUniformFloat(Shader shader, char *name, float value);
void SetUniformVec2(Shader shader, char *name, Vector2 value);
void SetUniformVec3(Shader shader, char *name, Vector3 value);
void SetUniformColor(Shader shader, char *name, Vector4 value);

//=========================================
// Meshes
//=========================================

void ApplyMesh(Mesh* mesh);
void FreeMesh(Mesh* mesh);

Mesh CreateQuadMesh();
Mesh CreateCubeMesh();
Mesh CreatePlaneMesh();
Mesh CreateUVSphereMesh();

void CalculateNormals(Mesh* mesh);

//========================================
// Models
//========================================
void Draw(Camera cam, Mesh mesh, PhongMaterial material, Matrix modelMatrix);

#endif // GRAPHICS_H