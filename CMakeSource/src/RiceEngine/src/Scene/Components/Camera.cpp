#include "pch.h"

#include <Rice/Scene/Components/Camera.hpp>

NSP_COMPONENTS_BEGIN

pTransform Camera::getTransform() {
    return getObject()->getComponent<Transform>();
}

Matrix4x4f Camera::getProjectionMatrix() {
    return Matrix4x4::Perspective(fov);
}

Matrix4x4f Camera::getViewMatrix() {
    return getTransform()->getInvTransformationMatrix();
}

NSP_COMPONENTS_END