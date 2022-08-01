#pragma once
#include "stdafx.hpp"

struct Quaternion;
struct Vector2;
struct Vector3;
struct Vector3f;
struct Matrix4x4f;

struct Matrix4x4 {
public:

	static Matrix4x4 perspective(double fov, double aspect = 1, double nearPlane = .001, double farPlane = 1000.);
	static Matrix4x4 ortographic(Vector2 size, double nearPlane = .001, double farPlane = 1000.);
	static Matrix4x4 ortographic(Vector3 size);

    Matrix4x4 inverse();
	static Matrix4x4 translation(Vector3 v);
	static Matrix4x4 scale(Vector3 v);
	static Matrix4x4 rotation(Quaternion q);
	static Matrix4x4 TRS(Vector3 translate, Quaternion rotate, Vector3 scale);

	Matrix4x4 transpose();

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

	Matrix4x4f(
		float _11, float _12, float _13, float _14,
		float _21, float _22, float _23, float _24,
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44) {

		c11 = _11;
		c12 = _12;
		c13 = _13;
		c14 = _14;

		c21 = _21;
		c22 = _22;
		c23 = _23;
		c24 = _24;

		c31 = _31;
		c32 = _32;
		c33 = _33;
		c34 = _34;

		c41 = _41;
		c42 = _42;
		c43 = _43;
		c44 = _44;
	}

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

	/*Matrix4x4f(const XMMATRIX& mat){
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
	}*/

	const static Matrix4x4f identity;

	float
		c11, c12, c13, c14,
		c21, c22, c23, c24,
		c31, c32, c33, c34,
		c41, c42, c43, c44;
};

Vector3f operator*(Vector3f vec, Matrix4x4f mat);
Matrix4x4f operator*(Matrix4x4f a, Matrix4x4f b);
