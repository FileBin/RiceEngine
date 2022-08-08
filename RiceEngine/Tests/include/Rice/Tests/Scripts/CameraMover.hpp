#pragma once

#include "Rice/Math/Vectors/Vector2.hpp"
#include "Rice/Scene/Components/MonoScript.hpp"

NSP_TESTS_BEGIN

class CameraMover : public virtual Components::MonoScript<CameraMover> {
    Vector3 speed = {0, 0, 0};

  public:
    void onEnable() override {
        auto scene = getScene();
        auto en = getEngine();

        auto ren = getSceneRender();
    }

    void onUpdate() override {
        auto scene = getClientScene();
        auto ren = getSceneRender();
        auto en = getClientEngine();
        auto cam = scene->getActiveCamera();
        auto input = en->getInputManager();

        auto cam_pos = cam->getTransform()->getPosition();
        auto cam_rot = cam->getTransform()->getRotation();
        auto fwd = cam_rot * Vector3::forward;

        Vector2 pos = input->getMouseInfinitePos();
        pos *= .3; // set sensivity
        pos.y = Math::min(Math::max(pos.y, -90.), 90.);
        pos.x = fmod(pos.x, 360);

        Vector3 mv = {0, 0, 0};

        if (input->getKey(KeyCode::D)) {
            mv.x += 1;
        }
        if (input->getKey(KeyCode::A)) {
            mv.x -= 1;
        }
        if (input->getKey(KeyCode::W)) {
            mv.z += 1;
        }
        if (input->getKey(KeyCode::S)) {
            mv.z -= 1;
        }
        if (input->getKey(KeyCode::Shift)) {
            mv.y -= 1;
        }
        if (input->getKey(KeyCode::Space)) {
            mv.y += 1;
        }

        mv.Qnormalize();

        auto yrot = Quaternion::fromAxisAngle(Vector3::up, pos.x);

        mv *= yrot;

        double dt = en->getDeltaTime();

        mv *= 20.;

        speed += mv * dt;

        Vector3 damp = speed;
        damp.Qnormalize();
        damp *= 15. * dt;

        if (damp.SqrLength() > speed.SqrLength()) {
            damp = speed;
        }
        speed -= damp;

        cam_pos += speed * dt;

        cam_rot =
            Quaternion::fromAxisAngle(yrot * Vector3::right, pos.y) * yrot;

        cam->getTransform()->setPosition(cam_pos);
        cam->getTransform()->setRotation(cam_rot);

        if (input->getKey(KeyCode::MouseLeft)) {
            input->lockMouse();
        }
    }
};

NSP_TESTS_END