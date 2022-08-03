#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "ctype.h"

// #include "SRMath.h"
#include "raymath.h"
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
// Initialization
//=========================================

GLFWwindow* CreateGLFWWindow(int width, int height, char* tittle);

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