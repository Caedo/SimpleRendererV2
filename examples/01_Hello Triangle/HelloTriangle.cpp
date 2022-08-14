#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

int main() {
    SRWindow window = InitializeWindow("Hello Triangle");

    Mesh mesh = {0};
    Vector3 verts[] = {
        {-0.8, -0.8, 0.0},
        { 0.8, -0.8, 0.0},
        { 0.0, 0.8,  0.0}
    };
    Vector4 colors[] = {
        {1, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 0, 1, 1}
    };
    int32_t tris[] = {0, 1, 2};

    mesh.vertices  = MakeSlice(verts, 0, 3);
    mesh.colors    = MakeSlice(colors, 0, 3);
    mesh.triangles = MakeSlice(tris, 0, 3);

    ApplyMesh(&mesh);

    Matrix identityMatrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    while(ShouldClose(&window) == false) {
        FrameStart(&window);

        DrawMesh(mesh, identityMatrix, VertexColorShader);

        FrameEnd(&window);
    }
    
    return 0;
}