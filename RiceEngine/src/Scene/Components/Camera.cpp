
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
Matrix4x4f Camera::getProjectionMatrix() {
    return Matrix4x4::perspective(fov, getEngine()->getAspectRatio(),
                                  near_plane, far_plane);
}

Matrix4x4f Camera::getViewMatrix() {
    return getTransform()->getInvTransformationMatrix();
}

NSP_COMPONENTS_END