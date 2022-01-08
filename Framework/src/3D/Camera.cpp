#include "pch.h"
#include <GameEngine\Camera.h>
#include <GameEngine\Matrixes.h>

namespace Game {
	Camera::Camera() {}

	Camera::~Camera() {}

	Matrix4x4 Camera::GetProjectionMatrix() {
		return Matrix4x4::Perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	Vector2 Camera::ToScreenPos(Vector3 p) {
		p = p * GetTransformationMatrix();
		p /= p.z;
		return { p.x, p.y };
	}

	bool Camera::IsPointVisible(Vector3 p) {
		auto screenP = ToScreenPos(p);
		if (screenP.x < -1 || screenP.x > 1 || screenP.y > 1 || screenP.y < -1) return false;
		return true;
	}

	Matrix4x4 Camera::GetTransformationMatrix() {
		auto mat = Matrix4x4::Translation(-position) * Matrix4x4::Rotation(rotation.Opposite());
		return mat;
	}
}