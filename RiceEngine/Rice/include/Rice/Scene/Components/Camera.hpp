#include "../../stdafx.hpp"

NSP_COMPONENTS_BEGIN
class Camera;
NSP_COMPONENTS_END

#pragma once
#include "Transform.hpp"

NSP_COMPONENTS_BEGIN

class Camera : public virtual Components::Component {
    dbl fov = 60;
    dbl near_plane = 0.01;
    dbl far_plane = 1000;

  public:
    Camera();

    ptr<Transform> getTransform();
    Matrix4x4 getViewMatrix();
    Matrix4x4 getProjectionMatrix();

    /*
     * Projection parameters. getProjectionMatrix() reads these on every call,
     * so a change is picked up by the next frame.
     *
     * setFov() takes *degrees* (as Matrix4x4::perspective does) and clamps to
     * (1, 179): tan(fov/2) is the scale factor, 0 deg / >=180 deg degenerate.
     * setNearPlane()/setFarPlane() clamp to a positive near plane that stays
     * below the far plane, so the depth range never inverts.
     */
    void setFov(dbl degrees);
    void setNearPlane(dbl value);
    void setFarPlane(dbl value);

    dbl getFov() const { return fov; }
    dbl getNearPlane() const { return near_plane; }
    dbl getFarPlane() const { return far_plane; }
};

NSP_COMPONENTS_END
