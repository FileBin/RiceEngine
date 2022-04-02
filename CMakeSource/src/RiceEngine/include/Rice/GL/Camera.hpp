#include "../stdafx.hpp"
NSP_SCENING_BEGIN
class Camera;
typedef SmartPtr<Camera> pCamera;
NSP_SCENING_END
#pragma once

#include "../Math.hpp"

NSP_SCENING_BEGIN

class Camera {
public:
	Camera();
	~Camera();

	Matrix4x4 getProjectionMatrix();
	Matrix4x4 getTransformationMatrix();

	void setFov(dbl _fov) {fov = _fov;}
	dbl getFov() {return fov;}

	void setAspectRatio(dbl aspectRatio) {aspect_ratio = aspectRatio;}
	dbl getAspectRatio() {return aspect_ratio;}

	Vector2 toScreenPos(Vector3 point);
	bool isPointVisible(Vector3 point);
	void setClippingPlanes(dbl nearPlane, dbl farPlane) {
		near_plane = nearPlane;
		far_plane = farPlane;
	}

	Vector3 position {0,0,0};
	Quaternion rotation {0,0,0,1};

private:
	dbl fov = 60.;
	dbl aspect_ratio = 1.;
	dbl near_plane = 0.01, far_plane = 1000.;
};

NSP_SCENING_END
