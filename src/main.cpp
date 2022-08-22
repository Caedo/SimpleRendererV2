#ifdef UNITY_BUILD
#include "unity.cpp"
#else
#include "SimpleRenderer.h"
#endif

#include <stdio.h>

int main()
{
    SRWindow* window = InitializeWindow(Str8Lit("Simple Renderer"));

    Shader shader = LoadShaderFromFile("shaders/testVert.vert", "shaders/testFrag.frag", &window->tempArena);
    UseShader(window, shader);

    Texture texture = LoadTextureAtPath((char*) "av.png", &window->tempArena);
    BindTexture(texture);

    Mesh cube = CreateCubeMesh();

    FaceCulling(window, true);
    DepthTest(window, true);

    Camera camera = CreatePerspective(90.0f, 0.01f, 1000, (float) window->width / window->height);
    // camera = CreateOrtographic(3, 0.1f, 1000.0, 800, 600);
    camera.position.x = -5;

    Matrix cube1Transform = MatrixScale(1, 0.1f, 0.1f) * MatrixTranslate(0.45f, 0, 0);
    Matrix cube2Transform = MatrixScale(0.1f, 1, 0.1f) * MatrixTranslate(0, 0.45f, 0);
    Matrix cube3Transform = MatrixScale(0.1f, 0.1f, 1) * MatrixTranslate(0, 0, 0.45f);

    while (ShouldClose(window) == false)
    {
        FrameStart(window);

        ClearColorAndDepthBuffer({0.5, 0.5, 0.5, 1});
 
        if(window->leftMouseBtnPressed) {
            camera.rotation.y += window->mouseDelta.x;
            camera.rotation.x += window->mouseDelta.y;
        }

        int verticalAxis = IsKeyDown(window, KEY_W) ? 1  :
                           IsKeyDown(window, KEY_S) ? -1 :
                           0;
        int horizontalAxis = IsKeyDown(window, KEY_A) ? 1  :
                             IsKeyDown(window, KEY_D) ? -1 :
                             0;

        Vector3 inputVector = Vector3Normalize(GetCameraRight(&camera) * (float) horizontalAxis + GetCameraForward(&camera) * (float) verticalAxis);
        camera.position = camera.position + inputVector * window->timeDelta * 5;

        DrawMesh(window, cube, camera, MatrixTranslate(0, 0, 0));
        DrawMesh(window, cube, camera, MatrixTranslate(0, 2, 0));
        DrawMesh(window, cube, camera, MatrixTranslate(0, 4, 0));

        BeginScreenSpace(window);

        DrawRect(window, {0, 0, 50, 80}, {1, 0, 0, 1});
        DrawRect(window, {55, 0, 50, 80}, {0, 1, 0, 1});
        DrawRect(window, {110, 0, 50, 80}, {0, 0, 0, 1});
        DrawRect(window, {165, 0, 50, 80}, {0, 1, 1, 1});
        DrawTexture(window, texture, {220, 0}, {0, 0});
        DrawTextureFragment(window, texture, {0, 0, 0.5, 0.5}, {450, 0, 50, 80});

        EndScreenSpace(window);

        FrameEnd(window);
    }

    glfwTerminate();

    return 0;
}