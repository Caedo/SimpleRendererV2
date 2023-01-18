#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

#define GLFW_DLL
#define GLFW_INCLUDE_NONE

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Memory.h"

#include "raymath.h"

#define FLOAT_MAX 3.402823466e+38F
#define FLOAT_MIN 1.175494351e-38F



struct Str8 {
    char* str;
    uint64_t length;

    char operator[] (int index) {
        assert(index < length && index >= 0);
        return str[index];
    }
};

enum class CameraType
{
    Perspective,
    Orthographic,
};

struct Camera
{
    Vector3 position;
    Vector3 rotation;

    Vector3 worldUp = {0, 1, 0};

    CameraType cameraType;

    float ortographicSize; // @Rename
    float fov;

    float aspect;

    float farPlane;
    float nearPlane;
};

/////////////////////////////////////

struct FileData {
    Str8 path;
    uint64_t changeTime;
};

//////////////////////////////////////

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

    FileData vertFileData;
    FileData fragFileData;
};

// Built in shaders for simple rendering
extern Shader ErrorShader;
extern Shader ColorShader;
extern Shader TextureShader;
extern Shader VertexColorShader;

extern Shader ScreenSpaceShader;

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

extern Texture ErrorTexture;

struct Rect {
    float x, y;
    float width, height;
};

////////////////////////////////////

// @NOTE: Those values are the same as GLFW ones, so it is
// trivial to use it in GLFW functions
// @TODO: naming convetion
enum Key {
    KEY_UNKNOWN       = 0,
    KEY_SPACE         = 32,
    KEY_APOSTROPHE    = 39, /* ' */
    KEY_COMMA         = 44, /* , */
    KEY_MINUS         = 45, /* - */
    KEY_PERIOD        = 46, /* . */
    KEY_SLASH         = 47, /* / */
    KEY_0             = 48,
    KEY_1             = 49,
    KEY_2             = 50,
    KEY_3             = 51,
    KEY_4             = 52,
    KEY_5             = 53,
    KEY_6             = 54,
    KEY_7             = 55,
    KEY_8             = 56,
    KEY_9             = 57,
    KEY_SEMICOLON     = 59, /* ; */
    KEY_EQUAL         = 61, /*        = */
    KEY_A             = 65,
    KEY_B             = 66,
    KEY_C             = 67,
    KEY_D             = 68,
    KEY_E             = 69,
    KEY_F             = 70,
    KEY_G             = 71,
    KEY_H             = 72,
    KEY_I             = 73,
    KEY_J             = 74,
    KEY_K             = 75,
    KEY_L             = 76,
    KEY_M             = 77,
    KEY_N             = 78,
    KEY_O             = 79,
    KEY_P             = 80,
    KEY_Q             = 81,
    KEY_R             = 82,
    KEY_S             = 83,
    KEY_T             = 84,
    KEY_U             = 85,
    KEY_V             = 86,
    KEY_W             = 87,
    KEY_X             = 88,
    KEY_Y             = 89,
    KEY_Z             = 90,
    KEY_LEFT_BRACKET  = 91, /* [ */
    KEY_BACKSLASH     = 92, /* \ */
    KEY_RIGHT_BRACKET = 93, /* ] */
    KEY_GRAVE_ACCENT  = 96, /* ` */
    KEY_WORLD_1       = 161, /* non-US #1 */
    KEY_WORLD_2       = 162, /* non-US #2 */
    KEY_ESCAPE        = 256,
    KEY_ENTER         = 257,
    KEY_TAB           = 258,
    KEY_BACKSPACE     = 259,
    KEY_INSERT        = 260,
    KEY_DELETE        = 261,
    KEY_RIGHT         = 262,
    KEY_LEFT          = 263,
    KEY_DOWN          = 264,
    KEY_UP            = 265,
    KEY_PAGE_UP       = 266,
    KEY_PAGE_DOWN     = 267,
    KEY_HOME          = 268,
    KEY_END           = 269,
    KEY_CAPS_LOCK     = 280,
    KEY_SCROLL_LOCK   = 281,
    KEY_NUM_LOCK      = 282,
    KEY_PRINT_SCREEN  = 283,
    KEY_PAUSE         = 284,
    KEY_F1            = 290,
    KEY_F2            = 291,
    KEY_F3            = 292,
    KEY_F4            = 293,
    KEY_F5            = 294,
    KEY_F6            = 295,
    KEY_F7            = 296,
    KEY_F8            = 297,
    KEY_F9            = 298,
    KEY_F10           = 299,
    KEY_F11           = 300,
    KEY_F12           = 301,
    KEY_F13           = 302,
    KEY_F14           = 303,
    KEY_F15           = 304,
    KEY_F16           = 305,
    KEY_F17           = 306,
    KEY_F18           = 307,
    KEY_F19           = 308,
    KEY_F20           = 309,
    KEY_F21           = 310,
    KEY_F22           = 311,
    KEY_F23           = 312,
    KEY_F24           = 313,
    KEY_F25           = 314,
    KEY_KP_0          = 320,
    KEY_KP_1          = 321,
    KEY_KP_2          = 322,
    KEY_KP_3          = 323,
    KEY_KP_4          = 324,
    KEY_KP_5          = 325,
    KEY_KP_6          = 326,
    KEY_KP_7          = 327,
    KEY_KP_8          = 328,
    KEY_KP_9          = 329,
    KEY_KP_DECIMAL    = 330,
    KEY_KP_DIVIDE     = 331,
    KEY_KP_MULTIPLY   = 332,
    KEY_KP_SUBTRACT   = 333,
    KEY_KP_ADD        = 334,
    KEY_KP_ENTER      = 335,
    KEY_KP_EQUAL      = 336,
    KEY_LEFT_SHIFT    = 340,
    KEY_LEFT_CONTROL  = 341,
    KEY_LEFT_ALT      = 342,
    KEY_LEFT_SUPER    = 343,
    KEY_RIGHT_SHIFT   = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT     = 346,
    KEY_RIGHT_SUPER   = 347,
    KEY_MENU          = 348,
    MAX_KEY_COUNT
};

enum class KeyState {
    Unknown,
    JustPressed,
    Down,
    JustReleased,
    Up,
};

struct Input {
    bool previousKeys[MAX_KEY_COUNT];
    bool currentKeys[MAX_KEY_COUNT];
    // @TODO: move mouse state here?
};

#define FRAME_TIMING_UPDATE_INTERVAL 0.5f

struct FrameTimeData {
    int frameCount;
    float frameTimeSum;

    float meanFrameTime;

    float minFrameTime;
    float maxFrameTime;
};

struct BatchVertex {
    Vector3 position;
    Vector2 uv;
    Vector4 color;
};

// @NOTE @TODO eyeballing this value, check if it's enought or to much
#define BATCH_MAX_SIZE 1024
struct BatchBuffer {
    size_t currentSize;
    BatchVertex vertices[BATCH_MAX_SIZE];

    uint32_t VAO;
    uint32_t VBO;
};

enum RenderState {
    Uninitialized,
    Frame,
    FrameEnded
};

// @NOTE: used to save current GL state
// so it can be restored after Screen Space
// functions finish rendering
struct ScreenSpaceContext {
    Shader shader;

    GLuint textureId;

    bool faceCulling;
    bool depthTest;
    bool blending;
};

struct SRWindow {
    GLFWwindow* glfwWin;

    MemoryArena persistentArena;
    MemoryArena tempArena;

    RenderState state;

    // GL state
    bool faceCulling;
    bool depthTest;
    bool blending;

    Shader currentShader;

    // @NOTE @TODO: it is enough to store only id?
    GLuint currentTextureId;

    FrameTimeData frameTimeData;

    // @Note: it is size of the framebuffer.
    // If you want the window size call glfwGetWindowFrameSize()
    int width;
    int height;

    double timeSinceStart;
    double previousFrameTime;
    float timeDelta;

    Vector2 mousePos;
    Vector2 mousePrevPos;
    Vector2 mouseDelta;

    bool leftMouseBtnPressed;
    bool rightMouseBtnPressed;

    Input input;

    // @Note: Used mainly for text and screen space rendering
    BatchBuffer batch;

    bool resizedThisFrame;
};

#define CharacterRange 383 // @TODO: dynamic range 
struct GlyphData {
    int codepoint;

    float pixelWidth;
    float pixelHeight;

    Rect atlasRect;

    float xOffset;
    float yOffset;

    int advanceX;
};

struct Font {
    int size;

    Texture atlas;
    GlyphData glyphData[CharacterRange - 32];
};

enum class UniformType {
    Float,
    Vec2,
    Vec3,
    Vec4,
    Int,
    IVec2,
    IVec3,
    IVec4,
    UInt,
    UVec2,
    UVec3,
    UVec4,
    Bool,
    BVec2,
    BVec3,
    BVec4,
    // Mat2,
    // Mat3,
    Mat4,
    // Mat2x3,
    // Mat2x4,
    // Mat3x2,
    // Mat3x4,
    // Mat4x2,
    // Mat4x3,
    Texture2D,
};

union UniformValue {
    float    floatValue[4];
    int32_t  intValue[4];
    uint32_t uintValue[4];

    GLuint texture;

    Matrix matrix;
};

struct Uniform {
    Str8 name;
    GLint location;

    int textureUnit;

    UniformType type;
    UniformValue value;
};

struct Material {
    Shader shader;

    int uniformsCount;
    Uniform uniforms[16];
};

//========================================

SRWindow* InitializeWindow(Str8 name, int width = 1270, int height = 720);
bool ShouldClose(SRWindow* window);

void FrameStart(SRWindow* window);
void FrameEnd(SRWindow* window);

//========================================
// Input
//========================================
KeyState GetKeyState(SRWindow* window, Key key);
bool IsKeyDown(SRWindow* window, Key key);
bool IsKeyUp(SRWindow* window, Key key);

//========================================
// FrameTiming
//========================================
void ShowFrameTime(SRWindow* window, Vector2 position);

//========================================
// Batch
// =======================================
void InitBatch(BatchBuffer* batch);
void AddBatchVertex(BatchBuffer* batch, Vector2 pos);
void AddBatchVertex(BatchBuffer* batch, BatchVertex v);
void AddBatchVertices(BatchBuffer*, Slice<BatchVertex> vertices);

void RenderBatch(SRWindow* window, BatchBuffer* batch);

//=========================================
// GL state
//=========================================

void FaceCulling(SRWindow* window, bool enabled);
void DepthTest(SRWindow* window, bool enabled);

void UseShader(SRWindow* window, Shader shader);

void ClearColorBuffer(Vector4 color);
void ClearDepthBuffer();
void ClearColorAndDepthBuffer(Vector4 color);

//=========================================
// Blending
//=========================================
void Blending(SRWindow* window, bool enabled);

void SetBlendingAlphaBlend(SRWindow* window);
void SetBlendingAdditive(SRWindow* window);
void SetBlendingPremultipliedAlpha(SRWindow* window);

//=========================================
// Shaders
//=========================================

Shader LoadShaderSource(const char *vertexShader, const char *fragmentShader);
Shader LoadShaderFromFile(const char *vertexPath, const char *fragmentPath, MemoryArena* arena);
bool ReloadShaderIfNecessary(Shader* shader, MemoryArena* arena);
void UnloadShader(Shader* shader);

void SetUniformFloat(Shader shader, char *name, float value);
void SetUniformVec2(Shader shader, char *name, Vector2 value);
void SetUniformVec3(Shader shader, char *name, Vector3 value);
void SetUniformColor(Shader shader, char *name, Vector4 value);

//=========================================
// Meshes
//=========================================

void ApplyMesh(Mesh* mesh);
void FreeMesh(Mesh* mesh);

Mesh CreateQuadMesh(MemoryArena* arena);
Mesh CreateCubeMesh(MemoryArena* arena);
Mesh CreatePlaneMesh(MemoryArena* arena);
Mesh CreateUVSphereMesh(MemoryArena* arena);

void CalculateNormals(Mesh* mesh);

//========================================
// Textures
//========================================
Texture LoadTextureAtPath(char* path, MemoryArena* arena);
Texture LoadTextureFromMemory(Slice<char> mem);

void BindTexture(SRWindow* window, Texture texture, uint32_t unit = 0);
void BindTexture(SRWindow* window, GLuint textureId, uint32_t unit = 0);

//========================================
// Drawing
//========================================
void DrawMesh(SRWindow* window, Mesh mesh, Matrix transform);
void DrawMesh(SRWindow* window, Mesh mesh, Camera camera, Matrix transform);

//========================================
// Screen Space drawing
//========================================
void DrawRect(SRWindow* window, Rect rect, Vector4 color);
void DrawTexture(SRWindow* window, Texture texture, Vector2 position, Vector2 origin);
void DrawTextureFragment(SRWindow* window, Texture texture, Rect source, Rect destination, Vector4 color = {1, 1, 1, 1});

//======================================
// Materials
//======================================

Material CreateMaterial(Shader shader);

void UseMaterial(SRWindow* window, Material* material);

void AddUniform(Material* material, Str8 name, UniformType type, UniformValue value);
void SetUniformValue(Material* material, Str8 name, UniformValue value);

int GetUniformIndex(Material* material, Str8 name);
Uniform GetUniform(Material* material, Str8 name);
UniformValue& GetUniformValue(Material* material, Str8 name, UniformType type);

float GetUniformValueFloat(Material* material, Str8 name);
Vector2 GetUniformValueVec2(Material* material, Str8 name);
Vector3 GetUniformValueVec3(Material* material, Str8 name);
Vector4 GetUniformValueVec4(Material* material, Str8 name);

int GetUniformValueInt(Material* material, Str8 name);
Vector2 GetUniformValueIVec2(Material* material, Str8 name);
Vector3 GetUniformValueIVec3(Material* material, Str8 name);
Vector4 GetUniformValueIVec4(Material* material, Str8 name);

uint32_t GetUniformValueUInt(Material* material, Str8 name);
Vector2 GetUniformValueUVec2(Material* material, Str8 name);
Vector3 GetUniformValueUVec3(Material* material, Str8 name);
Vector4 GetUniformValueUVec4(Material* material, Str8 name);

bool GetUniformValueBool(Material* material, Str8 name);
Vector2 GetUniformValueBVec2(Material* material, Str8 name);
Vector3 GetUniformValueBVec3(Material* material, Str8 name);
Vector4 GetUniformValueBVec4(Material* material, Str8 name);

Matrix GetUniformValueMatrix(Material* material, Str8 name);

//========================================
// Camera
//========================================

Camera CreateOrtographic(float ortographicSize, float nearPlane, float farPlane, float aspect);
Camera CreatePerspective(float fov, float nearPlane, float farPlane, float aspect);

Matrix GetView(Camera *cam);
Matrix GetProjection(Camera *cam);

Matrix GetVPMatrix(Camera *cam);

Vector3 GetCameraForward(Camera* cam);
Vector3 GetCameraRight(Camera* cam);
Vector3 GetCameraUp(Camera* cam);

//======================================
// Strings, text and fonts
//======================================
#define Str8Lit(c) Str8{(c), sizeof(c) - 1}

bool StringEqual(Str8 a, Str8 b);

Font LoadFontFromMemory(const unsigned char* data, int fontSize, MemoryArena* arena);
Font LoadFontAtPath(Str8 path, int fontSize, MemoryArena* arena);
Font LoadDefaultFont(int fontSize, MemoryArena* arena);

int GetCodepoint(Str8 text, int* advance);
int GetGlyphIndex(Str8 text, int* advance);

void DrawString(SRWindow* window, Str8 text, Font font, Vector2 position, Vector4 color = {0, 0, 0, 1});
int MeasureStringWidth(Str8 text, Font font);

// Temp platform specific definitions
uint64_t Win32_GetLastWriteTime(const char* filePath);
bool Win32_FileHasChanged(FileData fileData);

#endif