#include "Core.h"

bool KeyboardState[MAX_KEY_COUNT];

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

SRWindow InitializeWindow(char* name) {
    SRWindow win = {0};

    // @TODO: expose width and height to use code
    win.width = 800;
    win.height = 600;

    win.glfwWin = CreateGLFWWindow(800, 600, name);
    InitializeRenderer();

    win.tempArena = CreateArena();

    glfwSetKeyCallback(win.glfwWin, KeyCallback);

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
    window->mousePos.x = (float) currsorPosX / window->width;
    window->mousePos.y = (float) currsorPosY / window->height;

    window->mouseDelta = window->mousePos - window->mousePrevPos;

    // @TODO : that's not the best way to do this...
    window->leftMouseBtnPressed = glfwGetMouseButton(window->glfwWin, GLFW_MOUSE_BUTTON_1);
    window->rightMouseBtnPressed = glfwGetMouseButton(window->glfwWin, GLFW_MOUSE_BUTTON_2);

    // @TODO: Add option to configure this behaviour
    int escapeStatus = glfwGetKey(window->glfwWin, GLFW_KEY_ESCAPE);
    if (escapeStatus == GLFW_PRESS) {
        glfwSetWindowShouldClose(window->glfwWin, true);
    }

    memcpy(window->input.currentKeys,  KeyboardState,             sizeof(KeyboardState));
    memcpy(window->input.previousKeys, window->input.currentKeys, sizeof(KeyboardState));

    // @TODO: move to Graphics function
    glClearColor(0.6f, 0.6f, 0.6f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void FrameEnd(SRWindow* window) {
    ClearArena(&window->tempArena);
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

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        KeyboardState[key] = true;
    }
    else {
        KeyboardState[key] = false;
    }
}

KeyState GetKeyState(SRWindow* window, Key key) {
    bool curr = window->input.currentKeys[key];
    bool prev = window->input.previousKeys[key];

    if(curr == false && prev == false) {
        return KeyState::Up;
    }
    else if(curr == false && prev == true) {
        return KeyState::JustReleased;
    }
    else if(curr == true && prev == false) {
        return KeyState::JustPressed;
    }
    else {
        return KeyState::Down;
    }
}

bool IsKeyDown(SRWindow* window, Key key) {
    return window->input.currentKeys[key];
}

bool IsKeyUp(SRWindow* window, Key key) {
    return window->input.currentKeys[key] == false;
}