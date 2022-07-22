#include "../../stdafx.hpp"
#include "Rice/Math/Matrixes.hpp"
#include "Rice/Scene/Components/Transform.hpp"

NSP_COMPONENTS_BEGIN
class Camera;
typedef RefPtr<Camera> pCamera;
NSP_COMPONENTS_END

#pragma once

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