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

		auto rect = Util::GetWindowScreenSize(hwnd);
		auto center = rect * .5;
		center.x = lround(center.x);
		center.y = lround(center.y);

		Vector3 mv = { 0,0,0 };

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
		if (InputManager::GetKey(KeyCode::Shift)) {
			mv.y -= 1;
		}
		if (InputManager::GetKey(KeyCode::Space)) {
			mv.y += 1;
		}

		mv = cam.rotation * mv;
		mv *= en.GetDeltaTime() * 3.;

		cam.pos = cam.pos + mv;

		if (lock) {
			if (InputManager::GetKey(KeyCode::Escape)) {
				ShowCursor(true);
				lock = false;
			}


			auto delta = InputManager::GetMouseDelta();
			InputManager::SetMousePos(center);

			//Log::Debug(L"{}, {}", delta.x, delta.y);

			delta = delta * .2;
			pos = pos + delta;
			pos.y = min(max(pos.y, -90), 90);
			pos.x = fmod(pos.x, 360);
			auto yrot = Quaternion::FromAxisAngle(Vector3::up, pos.x);
			cam.rotation = Quaternion::FromAxisAngle(yrot * Vector3::right, pos.y) * yrot;

		} else if (InputManager::GetKey(KeyCode::MouseLeft)) {
			InputManager::SetMousePos(center);
			ShowCursor(false);
			lock = true;
		}
	}
};
