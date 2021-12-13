#include <GameEngine\Camera.h>
#include <GameEngine\Matrixes.h>

namespace Game {
	Camera::Camera() {}

	Camera::~Camera() {}

	Matrix4x4 Camera::GetProjectionMatrix() {
		return Matrix4x4::Perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	Matrix4x4 Camera::GetTransformationMatrix() {
		return Matrix4x4::Translation(-transform.pos) * Matrix4x4::Rotation(transform.rotation.Opposite());
	}
}