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

  public:
    Camera();

    ptr<Transform> getTransform();
    Matrix4x4f getViewMatrix();
    Matrix4x4f getProjectionMatrix();
};

NSP_COMPONENTS_END