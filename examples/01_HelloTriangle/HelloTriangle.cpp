#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Hello Triangle"));

    // Most 3D objects are created using triangles. In this context,
    // triangle is set of 3 vertices and all triangles are stored
    // in a structure called Mesh. One vertex, apart of postition may
    // be assigned other data like colors, normals etc.

    // GPU by default only renders vertices that are in <-1, 1> range.
    // This is called clip space
    Mesh mesh = {0};
    Vector3 verts[] = {
        {-0.8, -0.8, 0.0},
        { 0.8, -0.8, 0.0},
        { 0.0,  0.8, 0.0}
    };

    // We also assign colors to the vertices. Color, as you expect, are 
    // stored in the RGBA format, where each element is in <0, 1> range.
    Vector4 colors[] = {
        {1, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 0, 1, 1}
    };

    // One Vertex can be shared between several triangles, to do this, you have
    // to provide list of indices. Since we render only one triangle, our list
    // will have only 3 elements. 
    // Using raw OpenGL API you can skip this step and use vertices directly,
    // but shared triangles are generally more common, SimpleRenderer expects,
    // that you will use them.
    int32_t tris[] = {0, 1, 2};

    // Mesh uses Slices to store vertex data. Slice is just pointer + length
    mesh.vertices  = MakeSlice(verts, 0, 3);
    mesh.colors    = MakeSlice(colors, 0, 3);
    mesh.triangles = MakeSlice(tris, 0, 3);

    // ApplyMesh creates vertex buffers and uploads provided data to GPU, now
    // this mesh can be rendered
    ApplyMesh(&mesh);

    // More about matrices and why you need them in the next example
    Matrix identityMatrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    // More about shaders in the other example
    UseShader(window, VertexColorShader);

    while(ShouldClose(window) == false) {
        // FrameStart handles input and timings
        FrameStart(window);

        // We have to clear content of the framebuffer. Here we clear
        // color bit, using dark gray color
        ClearColorBuffer({0.1f, 0.1f, 0.1f, 0.f});

        // Now we can finally render our triangle
        DrawMesh(window, mesh, identityMatrix);

        FrameEnd(window);
    }
    
    return 0;
}