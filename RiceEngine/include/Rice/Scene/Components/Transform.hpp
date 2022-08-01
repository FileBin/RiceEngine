#include "../../stdafx.hpp"
#include <shared_mutex>

NSP_COMPONENTS_BEGIN
class Transform;
NSP_COMPONENTS_END

#pragma once

#include "../PackableComponentT.hpp"

NSP_COMPONENTS_BEGIN

class Transform
    : virtual public PackableComponentT<Transform> {

  private:
    std::shared_mutex mut;
    wptr<Transform> parent;
    Vector3 position = Vector3::zero;
    Quaternion rotation = Quaternion::identity;
    Vector3 scale = Vector3::one;

  public:
    // TODO void onEnable();

    Vector3 getPosition();

    Quaternion getRotation() {
        std::shared_lock l(mut);
        return rotation;
    }

    Vector3 getScale() {
        std::shared_lock l(mut);
        return scale;
    }

    void setPosition(Vector3 pos) {
        std::unique_lock l(mut);
        // TODO make set position global
        position = pos;
    }

    void setRotation(Quaternion q) {
        std::unique_lock l(mut);
        rotation = q;
    }

    void setScale(Vector3 s) {
        std::unique_lock l(mut);
        scale = s;
    }

    void setPRS(Vector3 pos = Vector3::zero,
                Quaternion rot = Quaternion::identity,
                Vector3 s = Vector3::one) {
        std::unique_lock l(mut);
        position = pos;
        rotation = rot;
        scale = s;
    }

    Matrix4x4 getTransformationMatrix() {
        std::shared_lock l(mut);

        auto parent_ref = parent.lock();

        if (parent_ref) {
            return Matrix4x4::TRS(position, rotation, scale) *
                   parent_ref->getTransformationMatrix();
        }
        return Matrix4x4::TRS(position, rotation, scale);
    }

    Matrix4x4 getInvTransformationMatrix() {
        std::shared_lock l(mut);

        auto parent_ref = parent.lock();
        // TODO make it better
        auto mat = Matrix4x4::translation(-position) *
                   Matrix4x4::rotation(rotation.opposite()) *
                   Matrix4x4::scale(1 / scale);
        if (parent_ref)
            return parent_ref->getInvTransformationMatrix() * mat;
        return mat;
    }
};

NSP_COMPONENTS_END
