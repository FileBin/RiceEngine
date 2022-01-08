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
		Vector2 ToScreenPos(Vector3 point);
		bool IsPointVisible(Vector3 point);
		void SetClippingPlanes(double nearPlane, double farPlane) {
			this->nearPlane = nearPlane;
			this->farPlane = farPlane;
		}

		Vector3 position{0,0,0};
		Quaternion rotation{0,0,0,1};

	private:
		double fov;
		double aspectRatio;
		double nearPlane = 0.01, farPlane = 1000.f;
	};
}