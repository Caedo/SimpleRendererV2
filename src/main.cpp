

#ifdef UNITY_BUILD
#include "unity.cpp"
#endif

bool mouseDown = false;
Vector2 previousMousePos;
Vector2 currentMousePos;

int main()
{
    SRWindow window = InitializeWindow("Window");

    FaceCulling(true);
    DepthTest(true);

    Shader shader = LoadShaderFromFile("shaders/testVert.vert", "shaders/testFrag.frag", &window.tempArena);

    Mesh cube = CreateCubeMesh();

    Camera camera = CreatePerspective(90.0f, 0.01f, 1000, 800, 600);
    // camera = CreateOrtographic(3, 0.1f, 1000.0, 800, 600);
    camera.position.x = -5;

    Matrix cube1Transform = MatrixScale(1, 0.1f, 0.1f) * MatrixTranslate(0.45f, 0, 0);
    Matrix cube2Transform = MatrixScale(0.1f, 1, 0.1f) * MatrixTranslate(0, 0.45f, 0);
    Matrix cube3Transform = MatrixScale(0.1f, 0.1f, 1) * MatrixTranslate(0, 0, 0.45f);

    while (ShouldClose(&window) == false)
    {
        FrameStart(&window);

        if(window.leftMouseBtnPressed) {
            camera.rotation.y += window.mouseDelta.x;
            camera.rotation.x += window.mouseDelta.y;
        }

        // camera.rotation.y += window.timeDelta;

        int verticalAxis = KeyboardState[KEY_W] ? 1  :
                           KeyboardState[KEY_S] ? -1 :
                           0;
        int horizontalAxis = KeyboardState[KEY_A] ? 1  :
                             KeyboardState[KEY_D] ? -1 :
                             0;

        Vector3 inputVector = Vector3Normalize(GetCameraRight(&camera) * (float) horizontalAxis + GetCameraForward(&camera) * (float) verticalAxis);
        camera.position = camera.position + inputVector * window.timeDelta * 5;

        DrawMesh(cube, camera, MatrixTranslate(0, 0, 0), shader);
        DrawMesh(cube, camera, MatrixTranslate(0, 2, 0), shader);
        DrawMesh(cube, camera, MatrixTranslate(0, 4, 0), shader);

        previousMousePos = currentMousePos;

        FrameEnd(&window);
    }

    glfwTerminate();

    return 0;
}