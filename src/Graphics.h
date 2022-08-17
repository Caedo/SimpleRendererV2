#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#include "Camera.h"
#include "Array.h"

#include "stb/stb_image.h"

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
    Slice<Vector4> colors;
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

struct Rect {
    float x, y;
    float width, height;
};

//=========================================
// Default shaders
//=========================================

const char* DefaultVertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNorm;\n"
"layout (location = 2) in vec2 aUV;\n"
"layout (location = 3) in vec4 aColor;\n"
"out vec3 pos;\n"
"out vec3 normal;\n"
"out vec2 uv;\n"
"out vec4 vertexColor;\n"
"uniform mat4 MVP;\n"
"void main() {\n"
"    pos = aPos;\n"
"    normal = aNorm;\n"
"    uv = aUV;\n"
"    vertexColor = aColor;\n"
"\n"
"    gl_Position = MVP * vec4(aPos, 1.0);\n"
"}";

const char* VertexColorShaderSource =
"#version 330 core\n"
"uniform vec4 tint;\n"
"in vec4 vertexColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vertexColor;\n"
"}";

const char* ColorShaderSource =
"#version 330 core\n"
"uniform vec4 tint;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = tint;\n"
"}";

const char* TextureShaderSource =
"#version 330 core\n"
"uniform vec4 tint;\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    vec4 color = texture(tex, uv);\n"
"    FragColor = color;\n"
"}";

const char* ErrorFragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1, 0, 1, 1);\n"
"}";


const char* ScreenSpaceVertexSource = 
R"###(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 aColor;

out vec2 uv;
out vec4 vertexColor;

vec3 ScreenToClip(vec3 pos) {
    vec2 p = pos.xy / vec2(800, 600);
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

out vec4 FragColor;

void main() {
    FragColor = vertexColor;
})###";

extern Shader ErrorShader;
extern Shader ColorShader;
extern Shader TextureShader;
extern Shader VertexColorShader;

extern Shader ScreenSpaceShader;

//=========================================
// Initialization
//=========================================

GLFWwindow* CreateGLFWWindow(int width, int height, char* tittle);
void InitalizeRenderer();

//=========================================
// GL state
//=========================================

void FaceCulling(SRWindow* window, bool enabled);
void DepthTest(SRWindow* window, bool enabled);
void UseShader(SRWindow* window, Shader shader);

//=========================================
// Shaders
//=========================================

Shader LoadShaderSource(const char *vertexShader, const char *fragmentShader);
Shader LoadShaderFromFile(const char *vertexPath, const char *fragmentPath, MemoryArena* arena);

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

Texture LoadTextureAtPath(char* path, MemoryArena* arena);
Texture LoadTextureFromMemory(Slice<char> mem);
void BindTexture(Texture texture);

//========================================
// Drawing
//========================================
void DrawMesh(Mesh mesh, Matrix transform);


//========================================
// Batch
// =======================================
void RenderBatch(SRWindow* window, Batch* batch);

//========================================
// Screen Space drawing
//========================================
void DrawRect(SRWindow* window, Rect rect, Vector4 color);
void DrawTexture(SRWindow* window, Texture texture, Vector2 position, Vector2 origin);
void DrawTextureFragment(SRWindow* window, Texture texture, Rect source, Rect destination);


#endif // GRAPHICS_H