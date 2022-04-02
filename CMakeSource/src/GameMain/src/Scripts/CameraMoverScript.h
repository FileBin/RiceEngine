#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <GameEngine\Components\Transform.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Components\Rigidbody.h>

#include "Util\MeshGenerator.h"
#include "ChunkGenerator.h"

#include <GameEngine\Vectors.h>
#include <GameEngine\Engine.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Camera.h>
#include <GameEngine\InputManager.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Util\DistanceEstimator.h>

using namespace Game;

class CameraMover : public MonoScript {
	bool lock = false;

	Vector2 pos = Vector2::zero;

	Vector3 speed = {0,0,0};

	SmartPtr<Material> mat;

	SmartPtr<Mesh> sphereMesh;

public:
	SmartPtr<ChunkGenerator> chunkGen;

	void Start() {
		auto& scene = GetScene();
		auto& en = GetEngine();

		auto& ren = scene.GetRender();

		mat = ren.CreateMaterial(L"SphereMat", ren.GetShader(L"Diffuse"), { Var(L"time"), Var(L"color"), Var(L"egst") });
		mat->SetVar<Vector4f>(L"color", { .6f, .6f, .5f, 1.f });
		mat->SetVar<Vector4f>(L"egst", { 0.3f, 2.f, 0.8f, 1.f });
		mat->UpdateBuffer();

		sphereMesh = MeshGenerator::GenerateMesh([](Vector3 p) { return p.Length() - 1.; }, { 4,4,4 });
	}


	void Update() {
		auto& ren = GetRender();
		auto& en = GetEngine();
		auto cam = ren.GetActiveCamera();

		auto fwd = cam->rotation * Vector3::forward;

		Vector3 mv = { 0,0,0 };

		if (InputManager::GetKey(KeyCode::D)) { mv.x += 1; }
		if (InputManager::GetKey(KeyCode::A)) { mv.x -= 1; }
		if (InputManager::GetKey(KeyCode::W)) { mv.z += 1; }
		if (InputManager::GetKey(KeyCode::S)) { mv.z -= 1; }
		if (InputManager::GetKey(KeyCode::Shift)) { mv.y -= 1; }
		if (InputManager::GetKey(KeyCode::Space)) { mv.y += 1; }

		mv.Qnormalize();

		auto yrot = Quaternion::fromAxisAngle(Vector3::up, pos.x);

		mv *= yrot;

		double dt = en.GetDeltaTime();
		
		mv *= 20.;

		speed += mv * dt;

		Vector3 damp = speed;
		damp.Qnormalize();
		damp *= 15. * dt;

		if(damp.SqrLength() > speed.SqrLength()){
			damp = speed;
		}
		speed -= damp;

		cam->position += speed * dt;

		//if (lock) {
			/*if (InputManager::GetKey(KeyCode::Escape)) {
				ShowCursor(true);
				lock = false;
			}*/
			auto delta = InputManager::GetMouseDelta();
			//InputManager::SetMousePos(center);

			delta = delta * .2;
			pos = pos + delta;
			pos.y = min(max(pos.y, -90), 90);
			pos.x = fmod(pos.x, 360);

			cam->rotation = Quaternion::fromAxisAngle(yrot * Vector3::right, pos.y) * yrot;
			SpawnSpheres();

		//} 
		/*if (InputManager::GetKey(KeyCode::MouseLeft)) {
			InputManager::LockMouse();
			InputManager::SetMousePos(center);
			ShowCursor(false);
			lock = true;
		}*/
	}

	bool click = false;
	std::chrono::steady_clock::time_point click_time = std::chrono::steady_clock::now();

	void SpawnSpheres() {
		if (InputManager::GetKey(KeyCode::MouseRight)) {
			//if (!click) {
			//	click = true;
				//SpawnSphere();
			//}
			if ((steady_clock::now() - click_time) > milliseconds(40)) {
				click_time = steady_clock::now();
				EraseVoxels();
			}
		} else {
			//click = false;
		}
	}

	void EraseVoxels() {
		auto physEn = GetScene().GetPhysEngine();
		auto cam = GetRender().GetActiveCamera();
		HitInfo info;
		if (physEn->Raycast(cam->position, cam->rotation * Vector3::forward, info, 128, .1, 5.)) {
			info.pos -= info.norm * 1.1;
			SDFunc func = [info](Vector3 p) {return sdSphere(p - info.pos, 2); };
			Vector3i minPos = info.pos - Vector3::one * 3;
			chunkGen->world->EraseVoxels(func, minPos, minPos + Vector3i(6, 6, 6));
			Log::log(Log::INFO, L"sus");
		}
	}

	void SpawnSphere() {
		//Sphere Init
		auto& scene = GetScene();
		auto& ren = GetRender();
		auto cam = ren.GetActiveCamera();
		auto sphere = scene.Instaniate();
		sphere->SetName(L"Sphere");
		auto sphereTr = new Transform();
		auto sphereBody = new Rigidbody();
		sphereBody->SetVelocity(cam->rotation * Vector3::forward * 10.);
		sphere->AddComponent(sphereTr);
		sphere->AddComponent(sphereBody);
		sphereTr->SetPosition(cam->position);
		auto mr = new ModelRender();
		sphere->AddComponent(mr);

		auto model = new Model();
		model->SetSubMeshesCount(1);
		model->SetSubMesh(new Mesh(*sphereMesh), 0);

		mr->SetModel(model);
		mr->SetMaterial(mat, 0);

		sphere->SetActive(true);
	}
};