#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

const char* Vertex = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aUV;\n"
"out vec3 pos;\n"
"out vec2 uv;\n"
"void main() {\n"
"    pos = aPos;\n"
"    uv = aUV;\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"}";

const char* Fragment =
"#version 330 core\n"
"uniform vec4 tint;\n"
"in vec3 pos;\n"
"in vec2 uv;\n"
"out vec4 FragColor;\n"
"uniform sampler2D tex;\n"
"void main() {\n"
"    vec3 color = texture(tex, uv).rgb;"
"    FragColor = vec4(color, 1);\n"
"}";


int main() {
    SRWindow window = InitializeWindow("Texture");

    Texture texture = LoadTextureAtPath("av.png");
    assert(texture.isValid);

    Shader shader = LoadShaderSource(Vertex, Fragment);
    assert(shader.isValid);

    Mesh mesh = {0};
    Vector3 verts[] = {
        {-0.8, -0.8, 0.0},
        { 0.8, -0.8, 0.0},
        { 0.8,  0.8, 0.0},
        {-0.8,  0.8, 0.0}
    };
    Vector2 uv[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    };
    int32_t tris[] = {0, 1, 2, 0, 2, 3};

    mesh.vertices  = MakeSlice(verts, 0, 4);
    mesh.uv        = MakeSlice(uv, 0, 4);
    mesh.triangles = MakeSlice(tris, 0, 6);

    ApplyMesh(&mesh);

    while(ShouldClose(&window) == false) {
        FrameStart(&window);

        glUseProgram(shader.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei) mesh.triangles.length, GL_UNSIGNED_INT, 0);

        FrameEnd(&window);
    }
    
    return 0;
}