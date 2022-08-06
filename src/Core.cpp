#include "Core.h"

SRWindow InitializeWindow(char* name) {
    SRWindow win = {0};

    win.glfwWin = CreateGLFWWindow(800, 600, "Simple Renderer");
    InitializeRenderer();

    return win;
}

bool ShouldClose(SRWindow* window) {
    return glfwWindowShouldClose(window->glfwWin);
}

void FrameStart(SRWindow* window) {
    window->timeSinceStart    = glfwGetTime();
    window->timeDelta         = (float)(window->timeSinceStart - window->previousFrameTime);
    window->previousFrameTime = window->timeSinceStart;

    glfwGetFramebufferSize(window->glfwWin, &window->width, &window->height);
    
    double currsorPosX;
    double currsorPosY;
    glfwGetCursorPos(window->glfwWin, &currsorPosX, &currsorPosY);

    window->mousePrevPos = window->mousePos;
    window->mousePos.X = (float) currsorPosX / window->width;
    window->mousePos.Y = (float) currsorPosX / window->height;

    window->mouseDelta = window->mousePos - window->mousePrevPos;

    // @TODO: move to Graphics function
    glClearColor(0.6f, 0.6f, 0.6f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // @TODO: Add option to configure this behaviour
    int escapeStatus = glfwGetKey(window->glfwWin, GLFW_KEY_ESCAPE);
    if (escapeStatus == GLFW_PRESS) {
        glfwSetWindowShouldClose(window->glfwWin, true);
    }
}

void FrameEnd(SRWindow* window) {
    glfwSwapBuffers(window->glfwWin);
    glfwPollEvents();
}


void FaceCulling(bool enabled) {
    if(enabled) {
        glEnable(GL_CULL_FACE);

        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
}

void DepthTest(bool enabled) {
    if(enabled) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}

// void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
//     if (button == GLFW_MOUSE_BUTTON_LEFT) {
//         if (action == GLFW_PRESS) {
//             mouseDown = true;
//         }
//         else if (action == GLFW_RELEASE) {
//             mouseDown = false;
//         }
//     }
// }

// void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//     if (key == GLFW_KEY_W) {
//         inputState.W_Pressed = action == GLFW_PRESS ? true :
//                                action == GLFW_RELEASE ? false :
//                                inputState.W_Pressed;
//     }

//     if (key == GLFW_KEY_S) {
//         inputState.S_Pressed = action == GLFW_PRESS ? true :
//                                action == GLFW_RELEASE ? false :
//                                inputState.S_Pressed;
//     }

//     if (key == GLFW_KEY_A) {
//         inputState.A_Pressed = action == GLFW_PRESS ? true :
//                                action == GLFW_RELEASE ? false :
//                                inputState.A_Pressed;
//     }

//     if (key == GLFW_KEY_D) {
//         inputState.D_Pressed = action == GLFW_PRESS ? true :
//                                action == GLFW_RELEASE ? false :
//                                inputState.D_Pressed;
//     }
// }
