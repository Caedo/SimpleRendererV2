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

    Mesh cube = CreateCubeMesh();

    FaceCulling(window, true);
    DepthTest(window, true);

    Camera camera = CreatePerspective(90.0f, 0.01f, 1000, (float) window->width / window->height);
    camera.position.x = -5;

    Font font = LoadFontAtPath(Str8Lit("Roboto-Regular.ttf"), 10, &window->tempArena);
    printf("%d, %d\n", font.atlas.width, font.atlas.height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        DrawString(window, Str8Lit("ĄąĆćĘęŁłÓóŚśŹźŻż"), font, {10, 40});

        FrameEnd(window);
    }

    glfwTerminate();

    return 0;
}