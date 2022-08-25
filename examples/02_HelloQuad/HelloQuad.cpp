#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Hello Quad"));

    Texture texture = LoadTextureAtPath("av.png", &window->tempArena);

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

    UseShader(window, TextureShader);
    BindTexture(texture);

    while(ShouldClose(window) == false) {
        FrameStart(window);

        ClearColorBuffer({0.9, 0.9, 0.9, 1});

        DrawMesh(window, mesh, MatrixIdentity());

        FrameEnd(window);
    }

    return 0;
}