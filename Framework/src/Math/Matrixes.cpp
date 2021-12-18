#include <GameEngine\Matrixes.h>
#include <GameEngine\Math.h>

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

Matrix4x4 operator*(Matrix4x4 A, Matrix4x4 b) {
    return {
        A.c11 * b.c11 + A.c21 * b.c12 + A.c31 * b.c13 + A.c41 * b.c14,
        A.c12 * b.c11 + A.c22 * b.c12 + A.c32 * b.c13 + A.c42 * b.c14,
        A.c13 * b.c11 + A.c23 * b.c12 + A.c33 * b.c13 + A.c43 * b.c14,
        A.c14 * b.c11 + A.c24 * b.c12 + A.c34 * b.c13 + A.c44 * b.c14,

        A.c11 * b.c21 + A.c21 * b.c22 + A.c31 * b.c23 + A.c41 * b.c24,
        A.c12 * b.c21 + A.c22 * b.c22 + A.c32 * b.c23 + A.c42 * b.c24,
        A.c13 * b.c21 + A.c23 * b.c22 + A.c33 * b.c23 + A.c43 * b.c24,
        A.c14 * b.c21 + A.c24 * b.c22 + A.c34 * b.c23 + A.c44 * b.c24,

        A.c11 * b.c31 + A.c21 * b.c32 + A.c31 * b.c33 + A.c41 * b.c34,
        A.c12 * b.c31 + A.c22 * b.c32 + A.c32 * b.c33 + A.c42 * b.c34,
        A.c13 * b.c31 + A.c23 * b.c32 + A.c33 * b.c33 + A.c43 * b.c34,
        A.c14 * b.c31 + A.c24 * b.c32 + A.c34 * b.c33 + A.c44 * b.c34,

        A.c11 * b.c41 + A.c21 * b.c42 + A.c31 * b.c43 + A.c41 * b.c44,
        A.c12 * b.c41 + A.c22 * b.c42 + A.c32 * b.c43 + A.c42 * b.c44,
        A.c13 * b.c41 + A.c23 * b.c42 + A.c33 * b.c43 + A.c43 * b.c44,
        A.c14 * b.c41 + A.c24 * b.c42 + A.c34 * b.c43 + A.c44 * b.c44,
    };
}

