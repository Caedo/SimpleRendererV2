#ifdef UNITY_BUILD
#include "unity.cpp"
#else
#include "SimpleRenderer.h"
#endif

int main()
{
    SRWindow* window = InitializeWindow(Str8Lit("Simple Renderer"));

    Camera camera = CreatePerspective(90.0f, 0.01f, 1000, (float) window->width / window->height);
    camera.position.x = -2;

    Font font = LoadFontAtPath(Str8Lit("Roboto-Regular.ttf"), 60, &window->tempArena);
    Str8 text = Str8Lit("Simple Renderer");
    int textWidth = MeasureStringWidth(text, font);

    Mesh cube = CreateCubeMesh();
    UseShader(window, VertexColorShader);

    Matrix cubeTransform = MatrixIdentity();

    FaceCulling(window, true);
    DepthTest(window, true);

    while (ShouldClose(window) == false) {
        FrameStart(window);

        cubeTransform = cubeTransform * MatrixRotate({0, 1, 1}, window->timeDelta);

        ClearColorAndDepthBuffer({0.5f, 0.5f, 0.8f, 1});

        DrawString(window, text, font, {(window->width - textWidth) / 2.f, 80});

        DrawMesh(window, cube, camera, cubeTransform);

        FrameEnd(window);
    }

    glfwTerminate();

    return 0;
}