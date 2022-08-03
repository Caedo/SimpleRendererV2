#include "Camera.h"

Camera CreateOrtographic(float ortographicSize, float nearPlane, float farPlane, int widthAspect, int heightAspect)
{
    Camera ret = {0};

    ret.ortographicSize = ortographicSize;
    ret.nearPlane = nearPlane;
    ret.farPlane = farPlane;
    ret.nearPlane = nearPlane;
    ret.aspect = (float)widthAspect / heightAspect;

    ret.cameraType = CameraType::Orthographic;

    return ret;
}

Camera CreatePerspective(float fov, float nearPlane, float farPlane, int widthAspect, int heightAspect)
{
    Camera ret = {0};

    ret.fov = fov;

    ret.nearPlane = nearPlane;
    ret.farPlane = farPlane;
    ret.nearPlane = nearPlane;
    ret.aspect = (float)widthAspect / heightAspect;

    ret.cameraType = CameraType::Perspective;

    return ret;
}

Matrix GetView(Camera *cam)
{
    // @TODO: add roll option...?

    float yaw = cam->rotation.y;
    float pitch = cam->rotation.x;

    Vector3 forward = { 0 };
    forward.x = cosf(yaw) * cosf(pitch);
    forward.y = sinf(pitch);
    forward.z = sinf(yaw) * cosf(pitch);

    Vector3 target = cam->position + Vector3Normalize(forward);

    return MatrixLookAt(cam->position, target, cam->worldUp);
}

Matrix GetProjection(Camera *cam)
{
    switch (cam->cameraType)
    {
        case Orthographic: {
            float orthoHeight = cam->ortographicSize;
            float orthoWidth = cam->aspect * orthoHeight;

            return MatrixOrtho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, cam->nearPlane, cam->farPlane);
        }
        break;

        case Perspective: {
            return MatrixPerspective(cam->fov * DEG2RAD, cam->aspect, cam->nearPlane, cam->farPlane);
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

    // @TODO @Copypasta: GetView
    Vector3 forward = { 0 };
    forward.x = cosf(yaw) * cosf(pitch);
    forward.y = sinf(pitch);
    forward.z = sinf(yaw) * cosf(pitch);

    return forward;
}

Vector3 GetCameraRight(Camera* cam) {
    float yaw = cam->rotation.y;
    float pitch = cam->rotation.x;

    // @TODO @Copypasta: GetView
    Vector3 forward = { 0 };
    forward.x = cosf(yaw) * cosf(pitch);
    forward.y = sinf(pitch);
    forward.z = sinf(yaw) * cosf(pitch);

    return Vector3Normalize(Vector3CrossProduct(cam->worldUp, forward));
}
