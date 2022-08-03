#ifndef CAMERA_H
#define CAMERA_H

// #include "SRMath.h"
#include "raymath.h"

enum CameraType
{
    Perspective,
    Orthographic,
};

struct Camera
{
    Vector3 position;
    Vector3 rotation;

    Vector3 worldUp = {0, 1, 0};

    CameraType cameraType;

    float ortographicSize; // @Rename
    float fov;

    float aspect;

    float farPlane;
    float nearPlane;
};

Camera CreateOrtographic(float ortographicSize, float nearPlane, float farPlane, int widthAspect, int heightAspect);
Camera CreatePerspective(float fov, float nearPlane, float farPlane, int widthAspect, int heightAspect);

Matrix GetView(Camera *cam);
Matrix GetProjection(Camera *cam);

Matrix GetVPMatrix(Camera *cam);

Vector3 GetCameraForward(Camera* cam);
Vector3 GetCameraRight(Camera* cam);

#endif // CAMERA_H