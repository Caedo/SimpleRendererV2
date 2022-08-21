#include "Core.h"

#include "imgui/imgui.h"

SRWindow windowInstance = {};

bool KeyboardState[MAX_KEY_COUNT];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ResizeCallback(GLFWwindow* window, int width, int height);

SRWindow* InitializeWindow(char* name) {
    // @TODO: expose width and height to use code
    windowInstance.width = 800;
    windowInstance.height = 600;

    windowInstance.glfwWin = CreateGLFWWindow(800, 600, name);
    InitializeRenderer();

    InitBatch(&windowInstance.screenSpaceBatch);

    windowInstance.tempArena = CreateArena();

    glfwSetKeyCallback(windowInstance.glfwWin, KeyCallback);
    glfwSetFramebufferSizeCallback(windowInstance.glfwWin, ResizeCallback);

    UseShader(&windowInstance, ErrorShader);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(windowInstance.glfwWin, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");


    return &windowInstance;
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

    memcpy(window->input.previousKeys, window->input.currentKeys, sizeof(window->input.previousKeys));

    // @TODO: move to Graphics function
    glClearColor(0.6f, 0.6f, 0.6f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void FrameEnd(SRWindow* window) {
    RenderBatch(window, &window->screenSpaceBatch);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ClearArena(&window->tempArena);
    glfwSwapBuffers(window->glfwWin);
    glfwPollEvents();

    window->resizedThisFrame = false;
}

void ResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    windowInstance.width = width;
    windowInstance.height = height;

    windowInstance.resizedThisFrame = true;
}

//========================================
// Input
// =======================================

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        windowInstance.input.currentKeys[key] = true;
    }
    else {
        windowInstance.input.currentKeys[key] = false;
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

//========================================
// Batch
// =======================================
void InitBatch(Batch* batch) {
    glGenVertexArrays(1, &batch->VAO);
    glBindVertexArray(batch->VAO);

    glGenBuffers(1, &batch->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BatchVertex), (void*)offsetof(BatchVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BatchVertex), (void*)offsetof(BatchVertex, uv));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BatchVertex), (void*)offsetof(BatchVertex, color));

    glBufferData(GL_ARRAY_BUFFER, sizeof(BatchVertex) * BATCH_MAX_SIZE, NULL, GL_STREAM_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &batch->whiteTextureId);
    glBindTexture(GL_TEXTURE_2D, batch->whiteTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint8_t data[] = {
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255,
        255, 255, 255, 255
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void AddBatchVertex(SRWindow* window, Batch* batch, Vector3 pos) {
    BatchVertex v = {
        pos,
        {0, 0},
        {1, 1, 1, 1}
    };

    AddBatchVertex(window, batch, v);
}

void AddBatchVertex(SRWindow* window, Batch* batch, BatchVertex v) {
    if(batch->currentSize + 1 >= BATCH_MAX_SIZE) {
        RenderBatch(window, batch);
    }

    batch->vertices[batch->currentSize] = v;
    batch->currentSize++;
}

void AddBatchVertices(SRWindow* window, Batch* batch, Slice<BatchVertex> vertices) {
    if(batch->currentSize + vertices.length >= BATCH_MAX_SIZE) {
        RenderBatch(window, batch);
    }

    size_t index = batch->currentSize;
    for(int i = 0; i < vertices.length; i++) {
        batch->vertices[index++] = vertices[i];
    }

    batch->currentSize = index;
}
