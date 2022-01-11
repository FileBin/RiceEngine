#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <GameEngine\Vectors.h>
#include <GameEngine\Engine.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Camera.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\Components\ModelRender.h>

using namespace Game;

class CameraMover : public MonoScript {
	bool lock = false;
	HWND hwnd;

	Vector2 pos = Vector2::zero;

	Vector3 speed = {0,0,0};

	void Start() {
		auto& scene = GetScene();
		auto& en = GetEngine();
		hwnd = en.GetHWND();
	}


	void Update() {
		auto& ren = GetRender();
		auto& en = GetEngine();
		auto cam = ren.GetActiveCamera();
		auto mouse = InputManager::GetMousePos();

		auto fwd = cam->rotation * Vector3::forward;

		auto rect = Util::GetWindowScreenSize(hwnd);
		auto center = rect * .5;
		center.x = lround(center.x);
		center.y = lround(center.y);

		Vector3 mv = { 0,0,0 };

		if (InputManager::GetKey(KeyCode::D)) { mv.x += 1; }
		if (InputManager::GetKey(KeyCode::A)) { mv.x -= 1; }
		if (InputManager::GetKey(KeyCode::W)) { mv.z += 1; }
		if (InputManager::GetKey(KeyCode::S)) { mv.z -= 1; }
		if (InputManager::GetKey(KeyCode::Shift)) { mv.y -= 1; }
		if (InputManager::GetKey(KeyCode::Space)) { mv.y += 1; }

		mv = mv.Normalized();
		mv *= cam->rotation;

		double dt = en.GetDeltaTime();
		
		mv *= 20.;

		speed += mv * dt;

		Vector3 damp = speed.Normalized() * 15. * dt;

		if(damp.SqrLength() > speed.SqrLength()){
			damp = speed;
		}
		speed -= damp;

		cam->position += speed * dt;

		if (lock) {
			if (InputManager::GetKey(KeyCode::Escape)) {
				ShowCursor(true);
				lock = false;
			}
			auto delta = InputManager::GetMouseDelta();
			InputManager::SetMousePos(center);

			delta = delta * .2;
			pos = pos + delta;
			pos.y = min(max(pos.y, -90), 90);
			pos.x = fmod(pos.x, 360);
			auto yrot = Quaternion::FromAxisAngle(Vector3::up, pos.x);
			cam->rotation = Quaternion::FromAxisAngle(yrot * Vector3::right, pos.y) * yrot;

		} else if (InputManager::GetKey(KeyCode::MouseLeft)) {
			InputManager::SetMousePos(center);
			ShowCursor(false);
			lock = true;
		}
	}
};