#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN
class PTR_PROTO(Camera);
NSP_COMPONENTS_END

#pragma once

#include "Transform.hpp"
#include "../Component.hpp"

NSP_COMPONENTS_BEGIN

class Camera : public Component {
    dbl fov = 60;
public:
    pTransform getTransform();
    Matrix4x4f getViewMatrix();
    Matrix4x4f getProjectionMatrix();
};

NSP_COMPONENTS_END