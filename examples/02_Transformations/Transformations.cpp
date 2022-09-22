#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Hello Quad"));

    // SimpleRenderer implements several prymitives that can 
    // be used for prototyping. That includes cubes, spheres, planes. etc,
    // Here we create cube mesh, which will be used to render several objects
    Mesh mesh = CreateCubeMesh();
    UseShader(window, VertexColorShader);

    // In the previous example I mentioned that GPU only renders vertices that are
    // in the clip space, and created triangle that had all positions in clip space already.
    // This is not very convininent, especially when we have dynamic camera. 
    // In that case we want to have some way to move or transform verices from one
    // coordinate space to another. Luckily, we can use matrices to do just that.

    Matrix matrices[8] = {0};

    // We can consider several coordinate spaces:
    // - Local/Object space: it is space local to rendered object with origin point, 
    //     at its position. All mesh vertices are stored in this space
    // - World space: main space of our program, almost all calculation are done here
    // - View space: space relative to the camera or "eyes" of our renderer 
    // - Clip space: space in which camera actually renders triangles
    // - Screen space: coordinates in pixels on the screen

    // To transform vertex from object space to clip space we will use MVP matrix, which stands
    // from Model View Projection. Those are names of matrices, each transform from one space to another:
    // Model tranforms from local to world space, View from world to view space and Projection from View 
    // space to Clip Space.
    // Two matrices are provided by Camera, and the last one is tranformation (position, rotation, scale) of
    // 3D object.

    // Here we create camera struct with Field of View of 75 degrees, near plane set to 0.01f, far plane to 1000,
    // and apect ration to aspect of our window.
    Camera camera = CreatePerspective(75, 0.01f, 1000.0f, (float) window->width / window->height);
    camera.position.x = -8;

    FaceCulling(window, true);

    while(ShouldClose(window) == false) {
        FrameStart(window);

        ClearColorBuffer({0.8f, 0.8f, 0.8f, 0.f});

        for(int i = 0; i < 8; i++) {
            matrices[i] = MatrixTranslate(0, sinf(window->timeSinceStart + i), i * 2 - 7);
        }

        Matrix vp = GetVPMatrix(&camera);
        for(int i = 0; i < 8; i++) {
            DrawMesh(window, mesh, vp * matrices[i]);
        }

        FrameEnd(window);
    }

    return 0;
}