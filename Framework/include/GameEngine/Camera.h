#pragma once
#include "Util.h"

namespace Game {
	using namespace Util;
	class Camera {
	public:
		Camera();
		~Camera();

		XMMATRIX GetProjectionMatrix();

		void SetFov(double fov) { this->fov = fov; }
		double GetFov() { return fov; }

		void SetAspectRatio(double ratio) { aspectRatio = ratio; }
		double GetAspectRatio() { return aspectRatio; }

		void SetClippingPlanes(double nearPlane, double farPlane) {
			this->nearPlane = nearPlane;
			this->farPlane = farPlane;
		}

		Transform transform;
	private:
		double fov;
		double aspectRatio;
		double nearPlane = 0.01, farPlane = 10000.f;
	};

	Camera::Camera() {}

	Camera::~Camera() {}

	inline XMMATRIX Camera::GetProjectionMatrix() {
		return XMMatrixPerspectiveFovRH(fov,aspectRatio, nearPlane, farPlane);
	}
}