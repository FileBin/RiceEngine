#include "pch.h"
#include "Rice/Scene/Object.hpp"
#include "Rice/Util/Exceptions/Exception.hpp"
#include <Rice/Scene/Components/Camera.hpp>

NSP_COMPONENTS_BEGIN

Camera::Camera() {}

ptr<Transform> Camera::getTransform() {
    auto transform = getObject()->getComponent<Transform>();
    if (!transform) {
        THROW_EXCEPTION("Object has no Transform component!");
    }
    return transform;
}
// TODO add aspect ratio
Matrix4x4 Camera::getProjectionMatrix() {
    return Matrix4x4::perspective(fov, getClientEngine()->getAspectRatio(),
                                  near_plane, far_plane);
}

void Camera::setFov(dbl degrees) {
    // tan(fov/2) scales x/y: 1..179 degrees keeps the matrix finite.
    fov = std::clamp(degrees, 1.0, 179.0);
}

void Camera::setNearPlane(dbl value) {
    // Positive and strictly inside the depth range.
    near_plane = std::clamp(value, 1e-6, far_plane * .5);
}

void Camera::setFarPlane(dbl value) {
    far_plane = std::max(value, near_plane * 2.);
}

Matrix4x4 Camera::getViewMatrix() {
    return getTransform()->getInvTransformationMatrix();
}

NSP_COMPONENTS_END