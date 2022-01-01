#include "pch.h"
#include <GameEngine\Camera.h>
#include <GameEngine\Matrixes.h>

namespace Game {
	Camera::Camera() {}

	Camera::~Camera() {}

	Matrix4x4 Camera::GetProjectionMatrix() {
		return Matrix4x4::Perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	Matrix4x4 Camera::GetTransformationMatrix() {
		auto mat = Matrix4x4::Translation(-position) * Matrix4x4::Rotation(rotation.Opposite());
		return mat;
	}
}