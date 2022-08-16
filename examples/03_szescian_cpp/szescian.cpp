#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif


int main() {
    SRWindow window = InitializeWindow("Szescian.cpp");

    Mesh mesh = CreateCubeMesh();
    ApplyMesh(&mesh);

    Camera camera = CreatePerspective(90, 0.01f, 1000.0f, window.width, window.height);
    camera.position.x = -2;

    FaceCulling(true);
    DepthTest(true);

    Texture tex = LoadTextureAtPath("av.png", &window.tempArena);
    BindTexture(tex);
    UseShader(&window, TextureShader);

    Matrix cubeRotation = MatrixIdentity();

    while(ShouldClose(&window) == false) {
        FrameStart(&window);

        // static float rotZ = 0;
        // static float rotY = 0;
        if(window.leftMouseBtnPressed) {
            float rotY = window.mouseDelta.x;
            float rotZ = window.mouseDelta.y;

            Vector3 worldY = InverseTransform(cubeRotation, Vector3{0, 1, 0});
            Vector3 worldZ = InverseTransform(cubeRotation, Vector3{0, 0, 1});

            Matrix mY = MatrixRotate(worldY, rotY);
            Matrix mZ = MatrixRotate(worldZ, rotZ);

            cubeRotation = cubeRotation * mZ * mY;
        }

        Matrix mvp = GetVPMatrix(&camera) * cubeRotation;

        DrawMesh(&window, mesh, mvp);

        FrameEnd(&window);
    }

    return 0;
}