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

	ModelRender* modelRender;

	void Start() {
		auto& scene = GetScene();
		auto& en = GetEngine();
		hwnd = en.GetHWND();
		modelRender = scene.GetObjectByName(L"Chunk").GetComponents<ModelRender>()[0];
	}

	void Update() {
		auto& ren = GetRender();
		auto& en = GetEngine();
		auto& cam = ren.GetCamera().transform;
		auto mouse = InputManager::GetMousePos();

		auto fwd = cam.rotation * Vector3::forward;

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

		mv *= cam.rotation;
		mv *= en.GetDeltaTime() * 3.;

		cam.pos = cam.pos + mv;

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
			cam.rotation = Quaternion::FromAxisAngle(yrot * Vector3::right, pos.y) * yrot;

		} else if (InputManager::GetKey(KeyCode::MouseLeft)) {
			InputManager::SetMousePos(center);
			ShowCursor(false);
			lock = true;
		}

		MoveVertices();
	}

	void MoveVertices() {
		/*auto& en = GetEngine();
		auto& model = modelRender->GetModel();
		auto& mesh = model.GetSubMesh(0);
		mesh.vertexBuffer[19].position.x =
			mesh.vertexBuffer[10].position.x =
			mesh.vertexBuffer[0].position.x += cos(en.GetTime()) * en.GetDeltaTime();

		GetRender().UpdateBuffer(&mesh);*/
	}
};