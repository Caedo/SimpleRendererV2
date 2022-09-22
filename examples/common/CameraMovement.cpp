#ifndef CAMERA_SPEED_X
#define CAMERA_SPEED_X 5
#endif

#ifndef CAMERA_SPEED_Z
#define CAMERA_SPEED_Z 5
#endif

#ifndef CAMERA_SPEED_Y
#define CAMERA_SPEED_Y 5
#endif

#ifndef MOUSE_SENSITIVITY_X
#define MOUSE_SENSITIVITY_X 80
#endif


#ifndef MOUSE_SENSITIVITY_Y
#define MOUSE_SENSITIVITY_Y 60
#endif

void MoveCamera(Camera* camera, SRWindow* window) {
    float forward = IsKeyDown(window, KEY_W) ?  1 :
                    IsKeyDown(window, KEY_S) ? -1 :
                                                0;

    float right = IsKeyDown(window, KEY_D) ?  1 :
                  IsKeyDown(window, KEY_A) ? -1 :
                                              0;

    float up = IsKeyDown(window, KEY_SPACE)      ?  1 :
               IsKeyDown(window, KEY_LEFT_SHIFT) ? -1 :
                                                    0;

    Vector2 input = Vector2Normalize(Vector2{forward, right});
    input.x *= CAMERA_SPEED_X * window->timeDelta;
    input.y *= CAMERA_SPEED_Z * window->timeDelta;

    camera->position += GetCameraForward(camera) * input.x + GetCameraRight(camera) * input.y;
    camera->position += GetCameraUp(camera) * up * CAMERA_SPEED_Y * window->timeDelta;

    if(window->leftMouseBtnPressed) {
        float mouseX = window->mouseDelta.x * MOUSE_SENSITIVITY_X * window->timeDelta;
        float mouseY = window->mouseDelta.y * MOUSE_SENSITIVITY_Y * window->timeDelta;

        camera->rotation.y += mouseX;
        camera->rotation.x += mouseY;
    }
}