#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN

class Transform;
typedef RefPtr<Transform> pTransform;

NSP_COMPONENTS_END

#pragma once

#include "../Component.hpp"

NSP_COMPONENTS_BEGIN

class Transform : public Component {
private:
  std::shared_mutex mut;
  pTransform parent = nullptr;
  Vector3 position = Vector3::zero;
  Quaternion rotation = Quaternion::identity;
  Vector3 scale = Vector3::one;

public:
  void onEnable() override;

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
    position = pos;
  }

  void SetRotation(Quaternion q) {
    std::unique_lock l(mut);
    rotation = q;
  }

  void SetScale(Vector3 s) {
    std::unique_lock l(mut);
    scale = s;
  }

  void SetPRS(Vector3 pos = Vector3::zero,
              Quaternion rot = Quaternion::identity, Vector3 s = Vector3::one) {
    std::unique_lock l(mut);
    position = pos;
    rotation = rot;
    scale = s;
  }

  Matrix4x4 GetTransformationMatrix() {
    std::shared_lock l(mut);
    if (parent != nullptr)
      return Matrix4x4::TRS(position, rotation, scale) *
             parent->GetTransformationMatrix();
    return Matrix4x4::TRS(position, rotation, scale);
  }

  Matrix4x4 GetInvTransformationMatrix() {
    std::shared_lock l(mut);
    auto mat = Matrix4x4::Translation(-position) *
               Matrix4x4::Rotation(rotation.Opposite()) *
               Matrix4x4::Scale(1 / scale);
    if (parent != nullptr)
      return parent->GetInvTransformationMatrix() * mat;
    return mat;
  }
};

NSP_COMPONENTS_END
