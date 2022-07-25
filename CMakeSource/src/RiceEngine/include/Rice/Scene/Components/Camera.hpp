#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN
class PTR_PROTO(Camera);
NSP_COMPONENTS_END

#pragma once

#include "../PackableComponent.hpp"
#include "Transform.hpp"

NSP_COMPONENTS_BEGIN

class Camera : public PackableComponent<Camera, RefTuple<dbl>, RefTuple<>> {
    dbl fov = 60;

  public:
    Camera();

    pTransform getTransform();
    Matrix4x4f getViewMatrix();
    Matrix4x4f getProjectionMatrix();
};

NSP_COMPONENTS_END