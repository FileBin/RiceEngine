#pragma once
#include "stdafx.h"
#include <xnamath.h>

struct Quaternion;
struct Vector3;

struct Matrix4x4 {
public:

	static Matrix4x4 Perspective(double fov, double aspect = 1, double nearPlane = .001, double farPlane = 1000.);

	static Matrix4x4 Translation(Vector3 v);
	static Matrix4x4 Scale(Vector3 v);
	static Matrix4x4 Rotation(Quaternion q);

	Matrix4x4 Transpose();

	const static Matrix4x4 identity;

	double 
	    c11, c12, c13, c14,
		c21, c22, c23, c24,
		c31, c32, c33, c34,
		c41, c42, c43, c44;
};

Vector3 operator*(Vector3 vec, Matrix4x4 mat);
Matrix4x4 operator*(Matrix4x4 m1, Matrix4x4 m2);

struct Matrix4x4f {
public:

	Matrix4x4f(const Matrix4x4& mat) {
		c11 = mat.c11;
		c12 = mat.c12;
		c13 = mat.c13;
		c14 = mat.c14;

		c21 = mat.c21;
		c22 = mat.c22;
		c23 = mat.c23;
		c24 = mat.c24;

		c31 = mat.c31;
		c32 = mat.c32;
		c33 = mat.c33;
		c34 = mat.c34;

		c41 = mat.c41;
		c42 = mat.c42;
		c43 = mat.c43;
		c44 = mat.c44;
	}

	Matrix4x4f(const XMMATRIX& mat){
		XMFLOAT4X4 m;
		XMStoreFloat4x4(&m, mat);

		c11 = m._11;
		c12 = m._12;
		c13 = m._13;
		c14 = m._14;

		c21 = m._21;
		c22 = m._22;
		c23 = m._23;
		c24 = m._24;

		c31 = m._31;
		c32 = m._32;
		c33 = m._33;
		c34 = m._34;

		c41 = m._41;
		c42 = m._42;
		c43 = m._43;
		c44 = m._44;
	}

	const static Matrix4x4f identity;

	float
		c11, c12, c13, c14,
		c21, c22, c23, c24,
		c31, c32, c33, c34,
		c41, c42, c43, c44;
};
