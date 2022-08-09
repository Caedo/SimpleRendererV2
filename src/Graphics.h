#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "ctype.h"

#include "Camera.h"
#include "Array.h"

#include "stb/stb_image.h"

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
    Slice<Vector3> vertices;
    Slice<Vector3> normals;
    Slice<Vector3> colors;
    Slice<Vector2> uv;
    Slice<int32_t> triangles;

    uint32_t VAO;
    uint32_t EBO;

    uint32_t positionsVBO;
    uint32_t normalsVBO;
    uint32_t colorsVBO;
    uint32_t uvVBO;
};

struct Texture {
    bool isValid;
    uint32_t id;

    int width;
    int height;
    int channels;

    // @TODO: create enums for this?
    // @TIDO: Split?
    int wrapType;

    // @TODO: split between magnifying and minifying?
    int filtering;

    // @TODO: mipmaps
    // @TODO: texture units
};

//=========================================
// Default shaders
//=========================================

const char* DefaultVertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNorm;\n"
"layout (location = 3) in vec4 aColor;\n"
"out vec3 pos;\n"
"out vec3 normal;\n"
"out vec4 vertexColor;\n"
"uniform mat4 MVP;\n"
"void main() {\n"
"    pos = aPos;\n"
"    normal = aNorm;\n"
"    vertexColor = aColor;\n"
"\n"
"    gl_Position = MVP * vec4(aPos, 1.0);\n"
"}";

const char* VertexColorShaderSource =
"#version 330 core\n"
"uniform vec4 tint;\n"
"in vec3 pos;\n"
"in vec4 vertexColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vertexColor;\n"
"}";

const char* ColorShaderSource =
"#version 330 core\n"
"uniform vec4 tint;\n"
"in vec3 pos;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = tint;\n"
"}";

const char* ErrorFragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1, 0, 1, 1);\n"
"}";

extern Shader ErrorShader;
extern Shader ColorShader;
extern Shader VertexColorShader;

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
// Textures
//========================================

Texture LoadTextureAtPath(char* path);
Texture LoadTextureFromMemory(Slice<char> mem);
void BindTexture(Texture texture);

//========================================
// Drawing
//========================================
void DrawMesh(Mesh mesh, Matrix transform, Shader shader);

// DrawMesh(mesh, material, camera, tranform) - standard function
// DrawMeshColor(mesh, camera, transform, color) - use default shader
// DrawMeshTexture(mesh, camera, transform, Texture) - use texture shader

// Screen space functions - I need them for microui anyway
// DrawRect(destination, color);
// DrawImage(destination, color);
// DrawImage(source, destination, color);

#endif // GRAPHICS_H