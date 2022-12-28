#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Szescian.cpp"));

    Mesh mesh = CreateCubeMesh(&window->persistentArena);

    Camera camera = CreatePerspective(90, 0.01f, 1000.0f, (float)window->width / window->height);
    camera.position.x = -2;

    FaceCulling(window, true);
    DepthTest(window, true);

    UseShader(window, VertexColorShader);

    Matrix cubeRotation = MatrixIdentity();

    while(ShouldClose(window) == false) {
        FrameStart(window);

        ClearColorAndDepthBuffer({0.8, 0.8, 0.8, 1});

        if(window->leftMouseBtnPressed) {
            float rotY = window->mouseDelta.x;
            float rotZ = window->mouseDelta.y;

            Vector3 worldY = InverseTransform(cubeRotation, Vector3{0, 1, 0});
            Vector3 worldZ = InverseTransform(cubeRotation, Vector3{0, 0, 1});

            Matrix mY = MatrixRotate(worldY, rotY);
            Matrix mZ = MatrixRotate(worldZ, rotZ);

            cubeRotation = cubeRotation * mZ * mY;
        }

        Matrix mvp = GetVPMatrix(&camera) * cubeRotation;

        DrawMesh(window, mesh, mvp);

        FrameEnd(window);
    }

    return 0;
}