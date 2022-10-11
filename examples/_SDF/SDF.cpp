#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

#include "../common/CameraMovement.cpp"


int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Signed Distance Field"));

    Camera camera = CreatePerspective(60, 0.01f, 1000.f, (float) window->width / window->height);
    camera.position.z = -10;
    camera.position.y = 4;
    camera.rotation.y = 90 * DEG2RAD;

    Mesh plane = CreatePlaneMesh();
    Mesh cube  = CreateCubeMesh();

    Matrix planeTransform = MatrixScale(3, 1, 3);
    Matrix cubeTransform  = MatrixTranslate(0, 3, 0) * MatrixScale(10, 10, 10);

    Vector3 mainLightDir   = { 1, -1, 0 };
    Vector3 mainLightColor = { 0.3f, 0.3f, 0.3f };

    Shader shader = LoadShaderFromFile("SDFVertex.glsl", "SDFFragment.glsl", &window->tempArena);

    Blending(window, true);
    SetBlendingAlphaBlend(window);

    UseShader(window, ColorShader);
    SetShaderUniform(window, "tint", Vector4{0.3, 0.3, 0.4, 1});

    Vector3 albedo     = {0.0, 0.0, 0.75};
    Vector3 lightColor = {0.1, 0.5, 0.3};
    Vector3 lightDir   = {-1, 0, 0};

    Vector3 lightRotation = {0, 0, 0};

    UseShader(window, shader);
    Vector2 r = {(float)window->width, (float)window->height};
    SetShaderUniform(window, "resolution", r);
    SetShaderUniform(window, "albedo",     albedo);
    SetShaderUniform(window, "lightColor", lightColor);
    SetShaderUniform(window, "lightDir",   lightDir);

    while(ShouldClose(window) == false) {
        FrameStart(window);
        ClearColorAndDepthBuffer({0.9f, 0.75f, 0.75f, 1});
        
        if(ReloadShaderIfNecessary(&shader, &window->tempArena)) {
            UseShader(window, shader);
            SetShaderUniform(window, "resolution", r);
            SetShaderUniform(window, "albedo",     albedo);
            SetShaderUniform(window, "lightColor", lightColor);
            SetShaderUniform(window, "lightDir",   lightDir);
        }

        // UI
        ImGui::SetNextWindowSize({370, 150}, ImGuiCond_Once);
        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Once);
        ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
        if(ImGui::Begin("Values")) {
            UseShader(window, shader);
            if(ImGui::ColorEdit3("Albedo", (float*) &albedo)) {
                SetShaderUniform(window, "albedo", albedo);
            }

            if(ImGui::ColorEdit3("Light Color", (float*) &lightColor)) {
                SetShaderUniform(window, "lightColor", lightColor);
            }

            bool xChanged = ImGui::SliderAngle("Light Rotation Z", &lightRotation.z, 180, 360);
            bool yChanged = ImGui::SliderAngle("Light Rotation Y", &lightRotation.y, 0, 360);
            if(xChanged || yChanged) {
                lightDir = MatrixRotateXYZ(lightRotation) * Vector3{-1, 0, 0};
                SetShaderUniform(window, "lightDir", lightDir);
            }

        }
        ImGui::End();

        // Camera
        camera.aspect = (float) window->width / window->height;
        MoveCamera(&camera, window);

        Matrix vp = GetVPMatrix(&camera);

        // Rendering
        UseShader(window, ColorShader);
        DrawMesh(window, plane, vp * planeTransform);

        UseShader(window, shader);

        SetShaderUniform(window, "cameraPos", camera.position);
        SetShaderUniform(window, "viewTarget", camera.position + GetCameraForward(&camera));
        SetShaderUniform(window, "viewUp", GetCameraUp(&camera));
        SetShaderUniform(window, "viewMatrix", GetView(&camera));
        SetShaderUniform(window, "iTime", (float)window->timeSinceStart);
        
        DrawMesh(window, cube, vp * cubeTransform);

        FrameEnd(window);
    }
}