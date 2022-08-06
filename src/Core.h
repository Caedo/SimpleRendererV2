#ifndef CORE_H
#define CORE_H

#define GLFW_DLL
#define GLFW_INCLUDE_NONE

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define HMM_PREFIX(name) name
#include "HandmadeMath.h"

typedef hmm_vec2 Vector2;
typedef hmm_vec3 Vector3;
typedef hmm_vec4 Vector4;

typedef hmm_mat4 Matrix;

struct SRWindow {
    GLFWwindow* glfwWin;

    // @Note: it is size of the framebuffer.
    // If you want the window size call glfwGetWindowSize()
    int width;
    int height;

    double timeSinceStart;
    double previousFrameTime;
    float timeDelta;

    Vector2 mousePos;
    Vector2 mousePrevPos;
    Vector2 mouseDelta;
};

SRWindow InitializeWindow(char* name);
bool ShouldClose(SRWindow* window);

void FrameStart(SRWindow* window);
void FrameEnd(SRWindow* window);

void FaceCulling(bool enabled);
void DepthTest(bool enabled);

// void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif