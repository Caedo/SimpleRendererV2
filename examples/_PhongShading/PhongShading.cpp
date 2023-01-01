#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

#include "../common/CameraMovement.cpp"

int main() {
    SRWindow* window = InitializeWindow(Str8Lit("Phong Shading"));

    Camera camera = CreatePerspective(60, 0.01f, 1000.f, (float) window->width / window->height);
    camera.position.x = -5;
    camera.position.y = 4;

    Mesh sphere = CreateUVSphereMesh(&window->persistentArena);
    Mesh plane = CreatePlaneMesh(&window->persistentArena);

    Matrix sphereTransform = MatrixTranslate(0, 2, 0);
    Matrix planeTransform = MatrixScale(3, 1, 3);

    Vector3 mainLightDir   = { 1, -1, 0 };
    Vector3 mainLightColor = { 0.3f, 0.3f, 0.3f };

    Vector3 pointLightPos   = { -2, 1, 4 };
    Vector3 pointLightColor = { 0, 0.8f, 0 };

    Shader shader = LoadShaderFromFile("PhongVertex.glsl", "PhongFragment.glsl", &window->tempArena);

    // Shader Uniforms
    UseShader(window, shader);

    SetShaderUniform(window, "mainLightDirection", mainLightDir);
    SetShaderUniform(window, "mainLightColor", mainLightColor);

    SetShaderUniform(window, "pointLightPos", pointLightPos);
    SetShaderUniform(window, "pointLightColor", pointLightColor);

    // Material 
    Material mat = CreateMaterial(shader);

    AddUniform(&mat, Str8Lit("albedo"),   UniformType::Vec3,  {0.4f, 0.52f, 0.8f});
    AddUniform(&mat, Str8Lit("diffuse"),  UniformType::Vec3,  {1, 1, 1});
    AddUniform(&mat, Str8Lit("specular"), UniformType::Vec3,  {1, 0.4f, 1});
    AddUniform(&mat, Str8Lit("shiness"),  UniformType::Float, {32.f});

    Material mat2 = mat;
    SetUniformValue(&mat2, Str8Lit("albedo"), {0.1f, 0.1f, 0.1f});

    FaceCulling(window, true);
    DepthTest(window, true);

    while(ShouldClose(window) == false) {
        FrameStart(window);
        ClearColorAndDepthBuffer({0.9f, 0.75f, 0.75f, 1});

        camera.aspect = (float) window->width / window->height;

        MoveCamera(&camera, window);
        pointLightPos = Vector3RotateByAxisAngle({-2, 1, 4}, {0, 1, 0}, window->timeSinceStart);

        SetShaderUniform(window, "cameraPos", camera.position);
        SetShaderUniform(window, "pointLightPos", pointLightPos);

        Matrix vp = GetVPMatrix(&camera);

        UseMaterial(window, &mat);
        SetShaderUniform(window, "model", sphereTransform);
        DrawMesh(window, sphere, vp * sphereTransform);

        UseMaterial(window, &mat2);
        SetShaderUniform(window, "model", planeTransform);
        DrawMesh(window, plane, vp * planeTransform);

        if(ImGui::ColorPicker3("Point light Color", (float*) &pointLightColor, ImGuiColorEditFlags_PickerHueWheel)) {
            SetShaderUniform(window, "pointLightColor", pointLightColor);
        }

        FrameEnd(window);
    }
}