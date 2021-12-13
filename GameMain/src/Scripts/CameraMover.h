#pragma once

#include <GameEngine\ScriptBase.h>
#include <GameEngine\ModelRender.h>
#include <GameEngine\InputManager.h>

using namespace Game;

class CameraMover : public ScriptBase {
public:
	ModelRenderer* render;

	bool lock = false;
	HWND hwnd;

	Vector2 pos = Vector2::zero;

	void Initialize() {
		auto& en = GetEngine();
		hwnd = en.GetHWND();
	}

	void Run() {
		auto& ren = *render;
		auto& en = GetEngine();
		auto& cam = ren.camera.transform;
		auto mouse = InputManager::GetMousePos();
		
		auto fwd = cam.rotation * Vector3::forward;

		auto center = Util::GetWindowScreenSize(hwnd) * .5;
		center.x = lround(center.x);
		center.y = lround(center.y);

		Vector3 mv = {0,0,0};

		if (InputManager::GetKey(KeyCode::Right)) {
			mv.x += 1;
		}
		if (InputManager::GetKey(KeyCode::Left)) {
			mv.x -= 1;
		}
		if (InputManager::GetKey(KeyCode::Up)) {
			mv.z += 1;
		}
		if (InputManager::GetKey(KeyCode::Down)) {
			mv.z -= 1;
		}

		mv = cam.rotation * mv;
		mv = mv * .1;

		cam.pos = cam.pos + mv;

		//Log::Debug(L"({0:0.4}; {1:0.4}; {2:0.4})", cam.pos.x, cam.pos.y, cam.pos.z);

		if (lock) {
			if (InputManager::GetKey(KeyCode::Escape)) {
				lock = false;
			}
			InputManager::SetMousePos(center);

			auto delta = mouse;
			delta = delta - center;

			delta = delta * .05;
			pos = pos + delta;
			auto yrot = Quaternion::FromAxisAngle(Vector3::up, pos.x);
			cam.rotation = Quaternion::FromAxisAngle(yrot * Vector3::right, pos.y) * yrot;//Quaternion::FromEulerAngles(pos.y, pos.x, 0);

		} else if (InputManager::GetKey(KeyCode::MouseLeft)) {
			InputManager::SetMousePos(center);
			lock = true;
		}
	}
};
