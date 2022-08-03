#ifndef SRMATH_H
#define SRMATH_H

#include <math.h>

#define PI 3.14159265358979323846

struct Vec2
{
    float x, y;
};

struct Vec3
{
    float x, y, z;
};

union Vec4 {
    struct
    {
        float x, y, z, w;
    };

    struct
    {
        float r, g, b, a;
    };
};

struct Mat4
{
    float m0, m4, m8, m12;
    float m1, m5, m9, m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
};

#ifndef vec3
#define vec3(x, y, z) \
    ((Vec3){(x), (y), (z)})
#endif

#ifndef vec2
#define vec2(x, y) \
    ((Vec2){(x), (y)})
#endif

#if SRMATH_IMPLEMENTATION
#define SRM inline
#else
#define SRM static inline
#endif

// Vec2 functions
SRM bool operator==(Vec2 a, Vec2 b)
{
    return a.x == b.x && a.y == b.y;
}

SRM bool operator!=(Vec2 a, Vec2 b)
{
    return !(a == b);
}

SRM Vec2 operator+(Vec2 a, Vec2 b)
{
    return {a.x + b.x, a.y + b.y};
}

SRM Vec2 operator-(Vec2 a, Vec2 b)
{
    return {a.x - b.x, a.y - b.y};
}

SRM Vec2 operator*(Vec2 a, float s)
{
    return {a.x * s, a.y * s};
}

SRM Vec2 operator/(Vec2 a, float s)
{
    return {a.x / s, a.y / s};
}

// Vec3 Functions
SRM bool operator==(Vec3 a, Vec3 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

SRM bool operator!=(Vec3 a, Vec3 b)
{
    return !(a == b);
}

SRM Vec3 operator+(Vec3 a, Vec3 b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

SRM Vec3 operator-(Vec3 a, Vec3 b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

SRM Vec3 operator*(Vec3 a, float s)
{
    return {a.x * s, a.y * s, a.z * s};
}

SRM Vec3 operator/(Vec3 a, float s)
{
    return {a.x / s, a.y / s, a.z / s};
}

SRM float Length(Vec2 v)
{
    return (float)sqrt(v.x * v.x + v.y * v.y);
}

SRM float Length(Vec3 v)
{
    return (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

SRM Vec3 Normalize(Vec3 v)
{
    return v / Length(v);
}

SRM Vec2 Normalize(Vec2 v)
{
    return v / Length(v);
}

SRM Vec3 Cross(Vec3 a, Vec3 b)
{
    Vec3 ret = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    return ret;
}

// MATRICES FUNCTIONS

SRM Mat4 OrthographicProjection(float left, float right, float bottom, float top, float near, float far)
{
    Mat4 res = {0};

    float horizontalDelta = right - left;
    float verticalDelta = top - bottom;
    float forwardDelta = far - near;

    res.m0 = 2 / horizontalDelta;
    res.m5 = 2 / verticalDelta;
    res.m10 = -2 / forwardDelta;

    res.m12 = -(left + right) / horizontalDelta;
    res.m13 = -(top + bottom) / verticalDelta;
    res.m14 = -(far + near) / forwardDelta;
    res.m15 = 1;

    return res;
}

SRM Mat4 FrustrumMatrix(float left, float right, float bottom, float top, float near, float far)
{
    Mat4 res = {};

    float horizontalDelta = right - left;
    float verticalDelta = top - bottom;
    float forwardDelta = far - near;

    res.m0 = (near * 2) / horizontalDelta;
    res.m5 = (near * 2) / verticalDelta;

    res.m8 = (left + right) / horizontalDelta;
    res.m9 = (top + bottom) / verticalDelta;
    res.m10 = -(far + near) / forwardDelta;
    res.m11 = -1;

    res.m14 = -(far * near * 2) / forwardDelta;

    return res;
}

SRM Mat4 PerspectiveProjection(float fov, float aspect, float near, float far)
{
    float top = near * tanf(fov * 0.5 * PI / 180);
    float right = top * aspect;

    return FrustrumMatrix(-right, right, -top, top, near, far);
}

SRM Mat4 Mat4Identity()
{
    Mat4 ret = {0};

    ret.m0 = 1;
    ret.m5 = 1;
    ret.m10 = 1;
    ret.m15 = 1;

    return ret;
}

SRM Mat4 operator*(Mat4 a, Mat4 b)
{
    Mat4 ret = {0};

    ret.m0 = a.m0 * b.m0 + a.m1 * b.m4 + a.m2 * b.m8 + a.m3 * b.m12;
    ret.m1 = a.m0 * b.m1 + a.m1 * b.m5 + a.m2 * b.m9 + a.m3 * b.m13;
    ret.m2 = a.m0 * b.m2 + a.m1 * b.m6 + a.m2 * b.m10 + a.m3 * b.m14;
    ret.m3 = a.m0 * b.m3 + a.m1 * b.m7 + a.m2 * b.m11 + a.m3 * b.m15;
    ret.m4 = a.m4 * b.m0 + a.m5 * b.m4 + a.m6 * b.m8 + a.m7 * b.m12;
    ret.m5 = a.m4 * b.m1 + a.m5 * b.m5 + a.m6 * b.m9 + a.m7 * b.m13;
    ret.m6 = a.m4 * b.m2 + a.m5 * b.m6 + a.m6 * b.m10 + a.m7 * b.m14;
    ret.m7 = a.m4 * b.m3 + a.m5 * b.m7 + a.m6 * b.m11 + a.m7 * b.m15;
    ret.m8 = a.m8 * b.m0 + a.m9 * b.m4 + a.m10 * b.m8 + a.m11 * b.m12;
    ret.m9 = a.m8 * b.m1 + a.m9 * b.m5 + a.m10 * b.m9 + a.m11 * b.m13;
    ret.m10 = a.m8 * b.m2 + a.m9 * b.m6 + a.m10 * b.m10 + a.m11 * b.m14;
    ret.m11 = a.m8 * b.m3 + a.m9 * b.m7 + a.m10 * b.m11 + a.m11 * b.m15;
    ret.m12 = a.m12 * b.m0 + a.m13 * b.m4 + a.m14 * b.m8 + a.m15 * b.m12;
    ret.m13 = a.m12 * b.m1 + a.m13 * b.m5 + a.m14 * b.m9 + a.m15 * b.m13;
    ret.m14 = a.m12 * b.m2 + a.m13 * b.m6 + a.m14 * b.m10 + a.m15 * b.m14;
    ret.m15 = a.m12 * b.m3 + a.m13 * b.m7 + a.m14 * b.m11 + a.m15 * b.m15;

    return ret;
}

SRM Mat4 LookAt(Vec3 position, Vec3 target, Vec3 up)
{
    // implementation based on: https://learnopengl.com/Getting-started/Camera
    Vec3 direction = Normalize(position - target);
    Vec3 right = Normalize(Cross(up, direction));
    Vec3 newUp = Normalize(Cross(direction, right));

    Mat4 rotMatrix = {0};
    rotMatrix.m0 = right.x;
    rotMatrix.m1 = newUp.x;
    rotMatrix.m2 = direction.x;
    rotMatrix.m3 = 0.f;
    rotMatrix.m4 = right.y;
    rotMatrix.m5 = newUp.y;
    rotMatrix.m6 = direction.y;
    rotMatrix.m7 = 0.f;
    rotMatrix.m8 = right.z;
    rotMatrix.m9 = newUp.z;
    rotMatrix.m10 = direction.z;
    rotMatrix.m11 = 0.f;
    rotMatrix.m12 = 0.f;
    rotMatrix.m13 = 0.f;
    rotMatrix.m14 = 0.f;
    rotMatrix.m15 = 1.f;

    Mat4 posMatrix = Mat4Identity();
    posMatrix.m12 = -position.x;
    posMatrix.m13 = -position.y;
    posMatrix.m14 = -position.z;

    //return rotMatrix * posMatrix;
    Mat4 res = posMatrix * rotMatrix;
    return res;
}

SRM Mat4 TranslateMatrix(Vec3 offset)
{
    return {1, 0, 0, offset.x,
            0, 1, 0, offset.y,
            0, 0, 1, offset.z,
            0, 0, 0, 1};
}

SRM Mat4 ScaleMatrix(Vec3 scale)
{
    return {scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1};
}

SRM Mat4 EulerToMatrix(Vec3 angles)
{

    // based on: https://catlikecoding.com/unity/tutorials/rendering/part-1/
    // but here I'm using 4x4 matrix.
    // Additional values are zeros, except m15 which equals 1

    Mat4 ret = Mat4Identity();

    float cosx = cosf(angles.x);
    float cosy = cosf(angles.y);
    float cosz = cosf(angles.z);

    float sinx = sinf(angles.x);
    float siny = sinf(angles.y);
    float sinz = sinf(angles.z);

    ret.m0 = cosy * cosz;
    ret.m1 = cosx * sinz + sinx * siny * cosz;
    ret.m2 = sinx * sinz - cosx * siny * cosz;

    ret.m4 = -cosy * sinz;
    ret.m5 = cosx * cosz - sinx * siny * sinz;
    ret.m6 = sinx * cosz + cosx * siny * sinz;

    ret.m8 = siny;
    ret.m9 = -sinx * cosy;
    ret.m10 = cosx * cosy;

    return ret;
}

SRM Mat4 AxisRotation(Vec3 axis, float angle)
{
    Mat4 res = {};

    axis = Normalize(axis);

    float cosr = cosf(angle);
    float sinr = sinf(angle);
    float cosrev = 1 - cosr;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    res.m0 = cosr + x * x * cosrev;
    res.m1 = y * x * cosrev + z * sinr;
    res.m2 = z * x * cosrev - y * sinr;
    res.m3 = 0;

    res.m4 = x * y * cosrev - z * sinr;
    res.m5 = y * y * cosrev + cosr;
    res.m6 = z * y * cosrev + x * sinr;
    res.m7 = 0.0f;

    res.m8 = x * z * cosrev + y * sinr;
    res.m9 = y * z * cosrev - x * sinr;
    res.m10 = z * z * cosrev + cosr;
    res.m11 = 0.0f;

    res.m12 = 0.0f;
    res.m13 = 0.0f;
    res.m14 = 0.0f;
    res.m15 = 1.0f;

    return res;
}

#endif