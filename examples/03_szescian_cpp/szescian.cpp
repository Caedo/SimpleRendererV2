#ifdef UNITY_BUILD
#include "../../src/unity.cpp"
#endif

// Matrix MatrixInvert(Matrix mat)
// {
//     Matrix result = { 0 };

//     float a00 = mat[0][0], a01 = mat[0][1], a02 = mat[0][2], a03 = mat[0][3];
//     float a10 = mat[1][0], a11 = mat[1][1], a12 = mat[1][2], a13 = mat[1][3];
//     float a20 = mat[2][0], a21 = mat[2][1], a22 = mat[2][2], a23 = mat[2][3];
//     float a30 = mat[3][0], a31 = mat[3][1], a32 = mat[3][2], a33 = mat[3][3];

//     float b00 = a00*a11 - a01*a10;
//     float b01 = a00*a12 - a02*a10;
//     float b02 = a00*a13 - a03*a10;
//     float b03 = a01*a12 - a02*a11;
//     float b04 = a01*a13 - a03*a11;
//     float b05 = a02*a13 - a03*a12;
//     float b06 = a20*a31 - a21*a30;
//     float b07 = a20*a32 - a22*a30;
//     float b08 = a20*a33 - a23*a30;
//     float b09 = a21*a32 - a22*a31;
//     float b10 = a21*a33 - a23*a31;
//     float b11 = a22*a33 - a23*a32;

//     // Calculate the invert determinant (inlined to avoid double-caching)
//     float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

//     result.Elements[0][0] = (a11*b11 - a12*b10 + a13*b09)*invDet;
//     result.Elements[1][0] = (-a01*b11 + a02*b10 - a03*b09)*invDet;
//     result.Elements[2][0] = (a31*b05 - a32*b04 + a33*b03)*invDet;
//     result.Elements[3][0] = (-a21*b05 + a22*b04 - a23*b03)*invDet;
//     result.Elements[0][1] = (-a10*b11 + a12*b08 - a13*b07)*invDet;
//     result.Elements[1][1] = (a00*b11 - a02*b08 + a03*b07)*invDet;
//     result.Elements[2][1] = (-a30*b05 + a32*b02 - a33*b01)*invDet;
//     result.Elements[3][1] = (a20*b05 - a22*b02 + a23*b01)*invDet;
//     result.Elements[0][2] = (a10*b10 - a11*b08 + a13*b06)*invDet;
//     result.Elements[1][2] = (-a00*b10 + a01*b08 - a03*b06)*invDet;
//     result.Elements[2][2] = (a30*b04 - a31*b02 + a33*b00)*invDet;
//     result.Elements[3][2] = (-a20*b04 + a21*b02 - a23*b00)*invDet;
//     result.Elements[0][3] = (-a10*b09 + a11*b07 - a12*b06)*invDet;
//     result.Elements[1][3] = (a00*b09 - a01*b07 + a02*b06)*invDet;
//     result.Elements[2][3] = (-a30*b03 + a31*b01 - a32*b00)*invDet;
//     result.Elements[3][3] = (a20*b03 - a21*b01 + a22*b00)*invDet;

//     return result;
// }


int main() {
    SRWindow window = InitializeWindow("Szescian.cpp");

    Mesh mesh = CreateCubeMesh();
    ApplyMesh(&mesh);

    Camera camera = CreatePerspective(90, 0.01f, 1000.0f, window.width, window.height);
    camera.position.x = -2;

    FaceCulling(true);
    DepthTest(true);

    Matrix cubeRotation = MatrixIdentity();

    while(ShouldClose(&window) == false) {
        FrameStart(&window);

        // static float rotZ = 0;
        // static float rotY = 0;
        if(window.leftMouseBtnPressed) {
            float rotY = window.mouseDelta.x;
            float rotZ = window.mouseDelta.y;

            Vector3 worldY = MatrixInvert(cubeRotation) * Vector3{0, 1, 0};
            Vector3 worldZ = MatrixInvert(cubeRotation) * Vector3{0, 0, 1};

            Matrix mY = MatrixRotate(worldY, rotY);
            Matrix mZ = MatrixRotate(worldZ, rotZ);

            cubeRotation = cubeRotation * mZ * mY;
        }

        Matrix mvp = GetVPMatrix(&camera) * cubeRotation;

        DrawMesh(mesh, mvp, VertexColorShader);

        FrameEnd(&window);
    }

    return 0;
}