#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN
class Camera;
NSP_COMPONENTS_END

#pragma once

#include "../PackableComponent.hpp"
#include "Transform.hpp"

NSP_COMPONENTS_BEGIN

class Camera : virtual public PackableComponentT<Camera> {
    dbl fov = 60;
    dbl near_plane = 0.01;
    dbl far_plane = 1000;

  public:
    Camera();

    ptr<Transform> getTransform();
    Matrix4x4 getViewMatrix();
    Matrix4x4 getProjectionMatrix();
};

NSP_COMPONENTS_END