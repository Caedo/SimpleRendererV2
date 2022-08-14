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

    ret.fov = fov * DEG2RAD;
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
