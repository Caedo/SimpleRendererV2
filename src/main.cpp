#include <stdio.h>
#include <assert.h>

#include "Core.h"

#include "Camera.h"
#include "Graphics.h"

Camera camera;
bool mouseDown = false;
Vector2 previousMousePos;
Vector2 currentMousePos;

int main()
{
    SRWindow window = InitializeWindow("Window");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Shader shader = LoadShaderFromFile("shaders/testVert.vert", "shaders/testFrag.frag");

    Mesh cube = CreateCubeMesh();

    PhongMaterial material1 = {};
    material1.shader = shader;

    material1.albedo = {0.9f, 0.2f, 0.2f};

    camera = CreatePerspective(60.0f, 0.01f, 1000, 800, 600);
    // camera = CreateOrtographic(3, 0.1f, 1000.0, 800, 600);
    camera.position.X = -10;

    Matrix cube1Transform = Scale({1, 0.1f, 0.1f}) * Translate({0.45f, 0, 0});
    Matrix cube2Transform = Scale({0.1f, 1, 0.1f}) * Translate({0, 0.45f, 0});
    Matrix cube3Transform = Scale({0.1f, 0.1f, 1}) * Translate({0, 0, 0.45f});

    while (ShouldClose(&window) == false)
    {
        FrameStart(&window);

        static bool mousePressed = false;
        int mouseBtn = glfwGetMouseButton(window.glfwWin, GLFW_MOUSE_BUTTON_1);

        if(mousePressed == false && mouseBtn == GLFW_PRESS) {
            mousePressed = true;
        }
        if(mousePressed == true && mouseBtn == GLFW_RELEASE) {
            mousePressed = false;
        }

        if(mousePressed) {
            camera.rotation.Y += window.mouseDelta.X;
            camera.rotation.Z += window.mouseDelta.Y;
        }

        int verticalAxis = glfwGetKey(window.glfwWin, GLFW_KEY_W) ? 1  :
                           glfwGetKey(window.glfwWin, GLFW_KEY_S) ? -1 :
                           0;
        int horizontalAxis = glfwGetKey(window.glfwWin, GLFW_KEY_A) ? 1  :
                             glfwGetKey(window.glfwWin, GLFW_KEY_D) ? -1 :
                             0;

        Vector3 inputVector = NormalizeVec3(GetCameraRight(&camera) * (float) horizontalAxis + GetCameraForward(&camera) * (float) verticalAxis);
        camera.position = camera.position + inputVector * window.timeDelta * 5;

        // DrawMesh(cube, camera, {0, 0, 0}, {1, 0, 0});
        // DrawMesh(cube, camera, {0, 2, 0}, {0, 1, 0});
        // DrawMesh(cube, camera, {0, 4, 0}, {0, 0, 1});

        Draw(camera, cube, material1, cube1Transform);

        previousMousePos = currentMousePos;

        FrameEnd(&window);
    }

    glfwTerminate();

    return 0;
}