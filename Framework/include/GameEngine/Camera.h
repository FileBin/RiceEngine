#pragma once

#include "Util3D.h"

struct Matrix4x4;

namespace Game {

	class Camera {
	public:
		Camera();
		~Camera();

		Matrix4x4 GetProjectionMatrix();
		Matrix4x4 GetTransformationMatrix();

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
		double nearPlane = 0.01, farPlane = 1000.f;
	};
}