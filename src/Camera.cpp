#include "Camera.h"

Camera CreateOrtographic(float ortographicSize, float nearPlane, float farPlane, int widthAspect, int heightAspect)
{
    Camera ret = {0};

    ret.ortographicSize = ortographicSize;
    ret.nearPlane = nearPlane;
    ret.farPlane = farPlane;
    ret.aspect = (float)widthAspect / heightAspect;

    ret.cameraType = CameraType::Orthographic;

    return ret;
}

Camera CreatePerspective(float fov, float nearPlane, float farPlane, int widthAspect, int heightAspect)
{
    Camera ret = {0};

    ret.fov = fov;
    ret.farPlane = farPlane;
    ret.nearPlane = nearPlane;
    ret.aspect = (float)widthAspect / heightAspect;

    ret.cameraType = CameraType::Perspective;

    return ret;
}

Matrix GetView(Camera *cam)
{
    // @TODO: add roll option...?

    float yaw = cam->rotation.Y;
    float pitch = cam->rotation.X;

    Vector3 forward = { 0 };
    forward.X = cosf(yaw) * cosf(pitch);
    forward.Y = sinf(pitch);
    forward.Z = sinf(yaw) * cosf(pitch);

    Vector3 target = cam->position + NormalizeVec3(forward);

    return LookAt(cam->position, target, cam->worldUp);
}

Matrix GetProjection(Camera *cam)
{
    switch (cam->cameraType)
    {
        case CameraType::Orthographic: {
            float orthoHeight = cam->ortographicSize;
            float orthoWidth = cam->aspect * orthoHeight;

            return Orthographic(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, cam->nearPlane, cam->farPlane);
        }
        break;

        case CameraType::Perspective: {
            return Perspective(cam->fov, cam->aspect, cam->nearPlane, cam->farPlane);
        }
        break;
    }

    return Mat4d(1);
}

Matrix GetVPMatrix(Camera *cam)
{
    return GetProjection(cam) * GetView(cam);
}

Vector3 GetCameraForward(Camera* cam) {
    float yaw = cam->rotation.Y;
    float pitch = cam->rotation.X;

    // @TODO @Copypasta: GetView
    Vector3 forward = { 0 };
    forward.X = cosf(yaw) * cosf(pitch);
    forward.Y = sinf(pitch);
    forward.Z = sinf(yaw) * cosf(pitch);

    return forward;
}

Vector3 GetCameraRight(Camera* cam) {
    float yaw = cam->rotation.Y;
    float pitch = cam->rotation.X;

    // @TODO @Copypasta: GetView
    Vector3 forward = { 0 };
    forward.X = cosf(yaw) * cosf(pitch);
    forward.Y = sinf(pitch);
    forward.Z = sinf(yaw) * cosf(pitch);

    return NormalizeVec3(Cross(cam->worldUp, forward));
}
