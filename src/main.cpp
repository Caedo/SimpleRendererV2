#include <stdio.h>
#include <assert.h>

#define GLFW_DLL
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SRMATH_IMPLEMETATION
#define DIR_LIGHTS_MAX 2

#define RAYMATH_STANDALONE
#define RAYMATH_IMPLEMENTATION
#include "raymath.h"
#undef RAYMATH_IMPLEMENTATION

#include "Camera.h"

#include "Graphics.h"

#include "Camera.cpp"
#include "Graphics.cpp"
#include "glad.c"

#include <windows.h>

Camera camera;
bool mouseDown = false;
Vector2 previousMousePos;
Vector2 currentMousePos;

struct InputState {
    bool W_Pressed;
    bool S_Pressed;
    bool A_Pressed;
    bool D_Pressed;
};

InputState inputState;

struct Time {
    double sinceStart;
    float delta;

    double previousFrameTime;
};

Time time;

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mouseDown = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mouseDown = false;
        }
    }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W)
    {
        inputState.W_Pressed = action == GLFW_PRESS ? true :
                               action == GLFW_RELEASE ? false :
                               inputState.W_Pressed;
    }

    if (key == GLFW_KEY_S)
    {
        inputState.S_Pressed = action == GLFW_PRESS ? true :
                               action == GLFW_RELEASE ? false :
                               inputState.S_Pressed;
    }

    if (key == GLFW_KEY_A)
    {
        inputState.A_Pressed = action == GLFW_PRESS ? true :
                               action == GLFW_RELEASE ? false :
                               inputState.A_Pressed;
    }

    if (key == GLFW_KEY_D)
    {
        inputState.D_Pressed = action == GLFW_PRESS ? true :
                               action == GLFW_RELEASE ? false :
                               inputState.D_Pressed;
    }
}

void FrameStart(GLFWwindow* window) {
    time.sinceStart        = glfwGetTime();
    time.delta             = (float)(time.sinceStart - time.previousFrameTime);
    time.previousFrameTime = time.sinceStart;

    glClearColor(0.6f, 0.6f, 0.6f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameEnd(GLFWwindow* window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main()
{
    GLFWwindow* window = CreateGLFWWindow(800, 600, "Simple Renderer");


    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Shader shader = LoadShaderFromFile("shaders/testVert.vert", "shaders/testFrag.frag");
    // @TODO: better handling shaders errors
    assert(shader.isValid);

    Mesh sphere = CreateUVSphereMesh();
    Mesh cube = CreateCubeMesh();
    Mesh plane = CreatePlaneMesh();

    PhongMaterial material1 = {};
    PhongMaterial material2 = {};
    material1.shader = shader;
    material2.shader = shader;

    material1.albedo = {0.9f, 0.2f, 0.2f};
    material2.albedo.z = 1.f;


    camera = CreatePerspective(60.0f, 0.01f, 1000, 800, 600);
    // camera = CreateOrtographic(3, 0.1f, 1000.0, 800, 600);
    camera.position.z = 4;
    camera.rotation.y = (float) -PI / 2;

    DirectionalLight light;
    light.direction = Vector3Normalize({-1.f, -0.5f, 0});
    light.color     = {0.7f, 1.f, 1.f};

    SetUniformVec3(shader, "dirLight.direction", light.direction);
    SetUniformVec3(shader, "dirLight.color", light.color);

    Matrix sphereTransform = MatrixIdentity();
    Matrix cubeTransform   = MatrixTranslate(-2.f, 0, 0);
    Matrix planeTransform  = MatrixTranslate(0, -1, 0);


    while (!glfwWindowShouldClose(window))
    {
        FrameStart(window);

        int escapeStatus = glfwGetKey(window, GLFW_KEY_ESCAPE);
        if (escapeStatus == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        double currPosX;
        double currPosY;
        glfwGetCursorPos(window, &currPosX, &currPosY);

        currentMousePos.x = (float)currPosX / 800;
        currentMousePos.y = (float)currPosY / 600;
        if (mouseDown)
        {
            Vector2 delta = currentMousePos - previousMousePos;
            camera.rotation.y += delta.x;
            camera.rotation.x += delta.y;
        }

        int verticalAxis = inputState.W_Pressed ? 1  :
                           inputState.S_Pressed ? -1 :
                           0;
        int horizontalAxis = inputState.A_Pressed ? 1  :
                             inputState.D_Pressed ? -1 :
                             0;

        Vector3 inputVector = Vector3Normalize(GetCameraRight(&camera) * (float) horizontalAxis + GetCameraForward(&camera) * (float) verticalAxis);
        camera.position = camera.position + inputVector * time.delta * 5;

        Draw(camera, sphere, material1, sphereTransform);
        Draw(camera, cube, material1, cubeTransform);
        Draw(camera, plane, material2, planeTransform);

        previousMousePos = currentMousePos;

        FrameEnd(window);
    }

    glfwTerminate();

    return 0;
}