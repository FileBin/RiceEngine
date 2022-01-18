#include "pch.h"
#include <GameEngine/Matrixes.h>
#include <GameEngine/Math.h>

#include <GameEngine/Vectors/Vector3.h>
#include <GameEngine/Vectors/Vector3f.h>
#include <GameEngine/Vectors/Quaternion.h>

const Matrix4x4 Matrix4x4::identity = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1,
};

const Matrix4x4f Matrix4x4f::identity = Matrix4x4::identity;

Matrix4x4 Matrix4x4::Perspective(double fov, double aspect, double nearPlane, double farPlane) {
    double yScale = 1. / Game::Math::Tan(fov / 2);
    double xScale = yScale / aspect;
    double nearmfar = farPlane - nearPlane;

    return {
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, (farPlane + nearPlane) / nearmfar, -(farPlane * nearPlane) / nearmfar,
        0, 0, 1, 0
    };
}

Matrix4x4 Matrix4x4::Ortographic(Vector2 size, double nearPlane, double farPlane) {
    double nearmfar = farPlane - nearPlane;

    return {
        2 / size.x, 0, 0, 0,
        0, 2 / size.y, 0, 0,
        0, 0, 2 / nearmfar, -(farPlane + nearPlane) / nearmfar,
        0, 0, 0, 1
    };
}

Matrix4x4 Matrix4x4::Ortographic(Vector3 size) {
    return {
        2/size.x, 0, 0, 0,
        0, 2/size.y, 0, 0,
        0, 0, 1/size.z, .5,
        0, 0, 0, 1
    };
}

Matrix4x4 Matrix4x4::Translation(Vector3 v) {
    return {
        1,0,0,v.x,
        0,1,0,v.y,
        0,0,1,v.z,
        0,0,0,1,
    };
}

Matrix4x4 Matrix4x4::Scale(Vector3 v) {
    return {
        v.x,0,0,0,
        0,v.y,0,0,
        0,0,v.z,0,
        0,0,0,1,
    };
}

Matrix4x4 Matrix4x4::Rotation(Quaternion q) {
    return {
        1 - 2 * (q.y*q.y + q.z*q.z), 2 * (q.x*q.y - q.w*q.z), 2 * (q.x*q.z + q.w * q.y), 0,
        2 * (q.x*q.y + q.w*q.z), 1 - 2 * (q.x*q.x + q.z*q.z), 2 * (q.y*q.z - q.w * q.x), 0,
        2 * (q.x*q.z - q.w*q.y), 2 * (q.y*q.z + q.w*q.x), 1 - 2 * (q.x*q.x + q.y * q.y), 0,
        0, 0, 0, 1,
    };
}

Matrix4x4 Matrix4x4::TRS(Vector3 translate, Quaternion rotate, Vector3 scale) {
    return Matrix4x4::Scale(scale) * Matrix4x4::Translation(translate) * Matrix4x4::Rotation(rotate);
}

Matrix4x4 Matrix4x4::Transpose() {
    return {
        c11, c21, c31, c41,
        c12, c22, c32, c42,
        c13, c23, c33, c43,
        c14, c24, c34, c44,
    };
}

Vector3 operator*(Vector3 vec, Matrix4x4 mat) {
    return {
        vec.x * mat.c11 + vec.y * mat.c12 + vec.z * mat.c13 + mat.c14,
        vec.x * mat.c21 + vec.y * mat.c22 + vec.z * mat.c23 + mat.c24,
        vec.x * mat.c31 + vec.y * mat.c32 + vec.z * mat.c33 + mat.c34,
    };
}


Vector3f operator*(Vector3f vec, Matrix4x4f mat) {
    return {
        vec.x * mat.c11 + vec.y * mat.c12 + vec.z * mat.c13 + mat.c14,
        vec.x * mat.c21 + vec.y * mat.c22 + vec.z * mat.c23 + mat.c24,
        vec.x * mat.c31 + vec.y * mat.c32 + vec.z * mat.c33 + mat.c34,
    };
}

Matrix4x4 operator*(Matrix4x4 a, Matrix4x4 b) {
    return {
        a.c11 * b.c11 + a.c21 * b.c12 + a.c31 * b.c13 + a.c41 * b.c14,
        a.c12 * b.c11 + a.c22 * b.c12 + a.c32 * b.c13 + a.c42 * b.c14,
        a.c13 * b.c11 + a.c23 * b.c12 + a.c33 * b.c13 + a.c43 * b.c14,
        a.c14 * b.c11 + a.c24 * b.c12 + a.c34 * b.c13 + a.c44 * b.c14,

        a.c11 * b.c21 + a.c21 * b.c22 + a.c31 * b.c23 + a.c41 * b.c24,
        a.c12 * b.c21 + a.c22 * b.c22 + a.c32 * b.c23 + a.c42 * b.c24,
        a.c13 * b.c21 + a.c23 * b.c22 + a.c33 * b.c23 + a.c43 * b.c24,
        a.c14 * b.c21 + a.c24 * b.c22 + a.c34 * b.c23 + a.c44 * b.c24,

        a.c11 * b.c31 + a.c21 * b.c32 + a.c31 * b.c33 + a.c41 * b.c34,
        a.c12 * b.c31 + a.c22 * b.c32 + a.c32 * b.c33 + a.c42 * b.c34,
        a.c13 * b.c31 + a.c23 * b.c32 + a.c33 * b.c33 + a.c43 * b.c34,
        a.c14 * b.c31 + a.c24 * b.c32 + a.c34 * b.c33 + a.c44 * b.c34,

        a.c11 * b.c41 + a.c21 * b.c42 + a.c31 * b.c43 + a.c41 * b.c44,
        a.c12 * b.c41 + a.c22 * b.c42 + a.c32 * b.c43 + a.c42 * b.c44,
        a.c13 * b.c41 + a.c23 * b.c42 + a.c33 * b.c43 + a.c43 * b.c44,
        a.c14 * b.c41 + a.c24 * b.c42 + a.c34 * b.c43 + a.c44 * b.c44,
    };
}

Matrix4x4f operator*(Matrix4x4f a, Matrix4x4f b) {
    return {
        a.c11 * b.c11 + a.c21 * b.c12 + a.c31 * b.c13 + a.c41 * b.c14,
        a.c12 * b.c11 + a.c22 * b.c12 + a.c32 * b.c13 + a.c42 * b.c14,
        a.c13 * b.c11 + a.c23 * b.c12 + a.c33 * b.c13 + a.c43 * b.c14,
        a.c14 * b.c11 + a.c24 * b.c12 + a.c34 * b.c13 + a.c44 * b.c14,

        a.c11 * b.c21 + a.c21 * b.c22 + a.c31 * b.c23 + a.c41 * b.c24,
        a.c12 * b.c21 + a.c22 * b.c22 + a.c32 * b.c23 + a.c42 * b.c24,
        a.c13 * b.c21 + a.c23 * b.c22 + a.c33 * b.c23 + a.c43 * b.c24,
        a.c14 * b.c21 + a.c24 * b.c22 + a.c34 * b.c23 + a.c44 * b.c24,

        a.c11 * b.c31 + a.c21 * b.c32 + a.c31 * b.c33 + a.c41 * b.c34,
        a.c12 * b.c31 + a.c22 * b.c32 + a.c32 * b.c33 + a.c42 * b.c34,
        a.c13 * b.c31 + a.c23 * b.c32 + a.c33 * b.c33 + a.c43 * b.c34,
        a.c14 * b.c31 + a.c24 * b.c32 + a.c34 * b.c33 + a.c44 * b.c34,

        a.c11 * b.c41 + a.c21 * b.c42 + a.c31 * b.c43 + a.c41 * b.c44,
        a.c12 * b.c41 + a.c22 * b.c42 + a.c32 * b.c43 + a.c42 * b.c44,
        a.c13 * b.c41 + a.c23 * b.c42 + a.c33 * b.c43 + a.c43 * b.c44,
        a.c14 * b.c41 + a.c24 * b.c42 + a.c34 * b.c43 + a.c44 * b.c44,
    };
}



