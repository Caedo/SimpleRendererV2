#include "Graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

Shader defaultShader;
Shader errorShader;

//=========================================
// Initialization
//=========================================

GLFWwindow* CreateGLFWWindow(int width, int height, char* tittle) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Simple Renderer", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to Initialie GLFW Window!!! \n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    return window;
}

void InitializeRenderer() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to Initialie GLAD!!! \n");
        glfwTerminate();
        return;
    }

    glfwSwapInterval(1);

    defaultShader = LoadShaderSource(DefaultVertexShader, DefaultFragmentShader);
    errorShader = LoadShaderSource(DefaultVertexShader, ErrorFragmentShader);

    assert(defaultShader.isValid);
    assert(errorShader.isValid);
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

Shader LoadShaderFromFile(const char *vertexPath, const char *fragmentPath)
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

        vertexSource = (char *)malloc(length + 1);
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

        fragmentSource = (char *)malloc(length + 1);
        fread(fragmentSource, 1, length, fragmentFile);

        fragmentSource[length] = '\0';
        fclose(fragmentFile);
    }
    else
    {
        // @TODO, LEAK vertexSource
        printf("[ERROR] Failed to open fragment shader source file at path: %s\n", fragmentPath);
        return {0};
    }

    Shader shader = LoadShaderSource(vertexSource, fragmentSource);

    free(vertexSource);
    free(fragmentSource);

    return shader;
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
        glUniform2f(loc, value.X, value.Y);
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
        glUniform3f(loc, value.X, value.Y, value.Z);
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
        // glUniform4f(loc, value.X, value.Y, value.Z, value.w);
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
        // glUniform4f(loc, value.X, value.Y, value.Z, value.w);
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
        glBufferData(GL_ARRAY_BUFFER, mesh->colors.length * sizeof(Vector3), mesh->colors.data, GL_STATIC_DRAW);

        glVertexAttribPointer(VertexColorIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
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
    mesh.triangles = AllocateSlice<int>(36);

    // front
    mesh.vertices[0] = {0.5f, 0.5f, 0.5f},   // top right 0
    mesh.vertices[1] = {0.5f, -0.5f, 0.5f},  // bottom right 1
    mesh.vertices[2] = {-0.5f, -0.5f, 0.5f}, // bottom left 2
    mesh.vertices[3] = {-0.5f, 0.5f, 0.5f},  // top left 3

    // back
    mesh.vertices[4] = {0.5f, 0.5f, -0.5f},   // top right 4
    mesh.vertices[5] = {0.5f, -0.5f, -0.5f},  // bottom right 5
    mesh.vertices[6] = {-0.5f, -0.5f, -0.5f}, // bottom left 6
    mesh.vertices[7] = {-0.5f, 0.5f, -0.5f},  // top left 7

    // left
    mesh.vertices[8] = {-0.5f, 0.5f, 0.5f},   // top front 8
    mesh.vertices[9] = {-0.5f, -0.5f, 0.5f},  // bottom front 9
    mesh.vertices[10] = {-0.5f, -0.5f, -0.5f}, // bottom back 10
    mesh.vertices[11] = {-0.5f, 0.5f, -0.5f},  // top back 11

    // right
    mesh.vertices[12] = {0.5f, 0.5f, 0.5f},   // top front 12
    mesh.vertices[13] = {0.5f, -0.5f, 0.5f},  // bottom front 13
    mesh.vertices[14] = {0.5f, -0.5f, -0.5f}, // bottom back 14
    mesh.vertices[15] = {0.5f, 0.5f, -0.5f},  // top back 15

    // top
    mesh.vertices[16] = {-0.5f, 0.5f, 0.5f},  // left front 16
    mesh.vertices[17] = {0.5f, 0.5f, 0.5f},   // right front 17
    mesh.vertices[18] = {0.5f, 0.5f, -0.5f},  // right back 18
    mesh.vertices[19] = {-0.5f, 0.5f, -0.5f}, // left back 19

    // bottom
    mesh.vertices[20] = {-0.5f, -0.5f, 0.5f},  // left front 20
    mesh.vertices[21] = {0.5f, -0.5f, 0.5f},   // right front 21
    mesh.vertices[22] = {0.5f, -0.5f, -0.5f},  // right back 22
    mesh.vertices[23] = {-0.5f, -0.5f, -0.5f}, // left back 23

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
        double p = HMM_PI * (double) (i + 1) / meshSize;
        double pSin = sin(p);
        double pCos = cos(p);

        for(int j = 0; j < meshSize; j++) {
            double a = 2.0 * HMM_PI * (double) j / meshSize;
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

        Vector3 normal = Cross(ab, ac);

        mesh->normals[indexA] = mesh->normals[indexA] + normal;
        mesh->normals[indexB] = mesh->normals[indexB] + normal;
        mesh->normals[indexC] = mesh->normals[indexC] + normal;
    }

    for (int i = 0; i < mesh->vertices.length; i++) {
        mesh->normals[i] = NormalizeVec3(mesh->normals[i]);
    }
}

//========================================
// Textures
//========================================

Texture LoadTextureAtPath(char* path) {
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

    char* fileData = (char *)malloc(length);
    fread(fileData, 1, length, file);
    fclose(file);

    ret = LoadTextureFromMemory(MakeSlice(fileData, 0, length));
    free(fileData);

    return ret;
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


//========================================
// Drawing
//========================================

void PrintMatrix(Matrix mat) {
    for(int i = 0; i < 4; i++) {
        printf("{%f, %f, %f, %f}\n", mat[i][0], mat[i][1], mat[i][2], mat[i][3]);
    }
    printf("\n");
}

void DrawMesh(Mesh mesh) {
    glUseProgram(defaultShader.id);
    
    Matrix mvp = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    uint32_t mvpLoc = glGetUniformLocation(defaultShader.id, "MVP");
    if(mvpLoc != -1)
        glUniformMatrix4fv(mvpLoc, 1, false, (const float *)(&mvp));

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.triangles.length, GL_UNSIGNED_INT, 0);
}

void DrawMesh(Mesh mesh, Camera camera, Vector3 position, Vector4 color) {
    glUseProgram(defaultShader.id);

    SetUniformColor(defaultShader, "tint", color);

    Matrix projection = GetProjection(&camera);
    Matrix view = GetView(&camera);

    Matrix mvp = projection * view * Translate(position);

    uint32_t mvpLoc = glGetUniformLocation(defaultShader.id, "MVP");
    if(mvpLoc != -1)
        glUniformMatrix4fv(mvpLoc, 1, false, (const float *)(&mvp));

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.triangles.length, GL_UNSIGNED_INT, 0);
}

void DrawMesh(Mesh mesh, Camera camera, Matrix transform, Shader shader) {
    uint32_t shaderId = shader.isValid ? shader.id : errorShader.id;
    glUseProgram(shaderId);

    Matrix projection = GetProjection(&camera);
    Matrix view = GetView(&camera);
    Matrix mvp = projection * view * transform;

    uint32_t mvpLoc = glGetUniformLocation(defaultShader.id, "MVP");
    if(mvpLoc != -1)
        glUniformMatrix4fv(mvpLoc, 1, false, (const float *)(&mvp));

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) mesh.triangles.length, GL_UNSIGNED_INT, 0);
}