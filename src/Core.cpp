#include "SimpleRenderer.h"
#include "Memory.h"

#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui.h"

SRWindow windowInstance = {};

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ResizeCallback(GLFWwindow* window, int width, int height);

SRWindow* InitializeWindow(Str8 name) {
    // @TODO: expose width and height to use code
    windowInstance.width = 800;
    windowInstance.height = 600;

    // GLFW Init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    windowInstance.glfwWin = glfwCreateWindow(800, 600, name.str, NULL, NULL);
    if (windowInstance.glfwWin == NULL)
    {
        printf("Failed to Initialie GLFW Window!!! \n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(windowInstance.glfwWin);

    glfwSetKeyCallback(windowInstance.glfwWin, KeyCallback);
    glfwSetFramebufferSizeCallback(windowInstance.glfwWin, ResizeCallback);

    windowInstance.tempArena = CreateArena();

    // GL Init
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to Initialie GLAD!!! \n");
        glfwTerminate();
        return NULL;
    }

    glfwSwapInterval(1);

    ErrorShader = LoadShaderSource(DefaultVertexShaderSource, ErrorFragmentShaderSource);
    ColorShader = LoadShaderSource(DefaultVertexShaderSource, ColorShaderSource);
    TextureShader = LoadShaderSource(DefaultVertexShaderSource, TextureShaderSource);
    VertexColorShader = LoadShaderSource(DefaultVertexShaderSource, VertexColorShaderSource);
    ScreenSpaceShader = LoadShaderSource(ScreenSpaceVertexSource, ScreenSpaceFragmentSource);

    assert(ErrorShader.isValid);
    assert(ColorShader.isValid);
    assert(TextureShader.isValid);
    assert(VertexColorShader.isValid);
    assert(ScreenSpaceShader.isValid);

    InitBatch(&windowInstance.screenSpaceBatch);
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
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(windowInstance.glfwWin, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return &windowInstance;
}

bool ShouldClose(SRWindow* window) {
    return glfwWindowShouldClose(window->glfwWin);
}

void FrameStart(SRWindow* window) {
    // @TODO: assert message
    assert(window->state == Uninitialized || window->state == FrameEnded);
    window->state = Frame;

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

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void FrameEnd(SRWindow* window) {
    assert(window->state == Frame);
    window->state = FrameEnded;

    RenderBatch(window, &window->screenSpaceBatch);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ClearArena(&window->tempArena);
    glfwSwapBuffers(window->glfwWin);
    glfwPollEvents();

    window->resizedThisFrame = false;
}


void BeginScreenSpace(SRWindow* window) {
    assert(window->state == Frame);
    window->state = ScreenSpace;

    SetUniformInt(ScreenSpaceShader, "framebufferWidth", window->width);
    SetUniformInt(ScreenSpaceShader, "framebufferHeight", window->height);
}

void EndScreenSpace(SRWindow* window) {
    assert(window->state == ScreenSpace);
    window->state = Frame;

    RenderBatch(window, &window->screenSpaceBatch);
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


//========================================
// Camera
// =======================================

Camera CreateOrtographic(float ortographicSize, float nearPlane, float farPlane, float aspect)
{
    Camera ret = {0};

    ret.ortographicSize = ortographicSize;
    ret.nearPlane = nearPlane;
    ret.farPlane = farPlane;
    ret.aspect = aspect;

    ret.cameraType = CameraType::Orthographic;

    return ret;
}

Camera CreatePerspective(float fov, float nearPlane, float farPlane, float aspect)
{
    Camera ret = {0};

    ret.fov = fov * DEG2RAD;
    ret.farPlane = farPlane;
    ret.nearPlane = nearPlane;
    ret.aspect = aspect;

    ret.cameraType = CameraType::Perspective;

    return ret;
}

Matrix GetView(Camera *cam)
{
    // @TODO: add roll option...?

    float yaw = cam->rotation.y;
    float pitch = cam->rotation.x;

    Vector3 forward = GetCameraForward(cam);
    Vector3 target = cam->position + Vector3Normalize(forward);

    return MatrixLookAt(cam->position, target, cam->worldUp);
}

Matrix GetProjection(Camera *cam)
{
    switch (cam->cameraType)
    {
        case CameraType::Orthographic: {
            float orthoHeight = cam->ortographicSize;
            float orthoWidth = cam->aspect * orthoHeight;

            return MatrixOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, cam->nearPlane, cam->farPlane);
        }
        break;

        case CameraType::Perspective: {
            return MatrixPerspective(cam->fov, cam->aspect, cam->nearPlane, cam->farPlane);
        }
        break;
    }

    return MatrixIdentity();
}

Matrix GetVPMatrix(Camera *cam)
{
    return GetProjection(cam) * GetView(cam);
}

Vector3 GetCameraForward(Camera* cam) {
    float yaw = cam->rotation.y;
    float pitch = cam->rotation.x;

    Vector3 forward = { 0 };
    forward.x = cosf(yaw) * cosf(pitch);
    forward.y = sinf(pitch);
    forward.z = sinf(yaw) * cosf(pitch);

    return forward;
}

Vector3 GetCameraRight(Camera* cam) {
    float yaw = cam->rotation.y;
    float pitch = cam->rotation.x;

    Vector3 forward = GetCameraForward(cam);
    return Vector3Normalize(Vector3CrossProduct(cam->worldUp, forward));
}
