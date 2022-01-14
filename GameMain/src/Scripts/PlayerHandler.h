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

class PlayerHandler : public MonoScript {

	Vector2 pos = Vector2::zero;

	SmartPtr<Rigidbody> playerBody;
	SmartPtr<Transform> transform;

	bool grounded = false;
public:
	SmartPtr<ChunkGenerator> chunkGen;
	std::chrono::steady_clock::time_point click_time = std::chrono::steady_clock::now();
	void Start() {
		auto& scene = GetScene();
		auto& en = GetEngine();
		auto& obj = GetSceneObject();

		auto& ren = scene.GetRender();
		transform = obj.GetComponents<Transform>()[0];
		playerBody = obj.GetComponents<Rigidbody>()[0];
	}

	void Update() {
		auto& ren = GetRender();
		auto& en = GetEngine();
		auto cam = ren.GetActiveCamera();

		Vector3 mv = { 0,0,0 };

		if (InputManager::GetKey(KeyCode::D)) { mv.x += 1; }
		if (InputManager::GetKey(KeyCode::A)) { mv.x -= 1; }
		if (InputManager::GetKey(KeyCode::W)) { mv.z += 1; }
		if (InputManager::GetKey(KeyCode::S)) { mv.z -= 1; }

		mv.Qnormalize();

		if (InputManager::GetKey(KeyCode::Shift)) { mv *= .5; }

		auto yrot = Quaternion::FromAxisAngle(Vector3::up, pos.x);

		mv *= yrot;

		double dt = en.GetDeltaTime();
		auto physEn = GetScene().GetPhysEngine();
		HitInfo hit;
		grounded = physEn->Raycast(transform->position, Vector3::down, hit, 32, .6, 1.);

		auto tangent = Vector3::Dot(Vector3::up, hit.norm);

		auto vel = playerBody->GetVelocity();
		auto speed = vel.Length();
		auto targetvel = mv * 10.;
		auto deltavel = targetvel - vel;
		if (!grounded) {
			deltavel = Vector3::ProjectOnPlane(deltavel, Vector3::up);
		}

		bool stop = mv.Length() < .5 && grounded;

		if (stop) {
			deltavel *= Math::Lerp(.1, 1., Math::Clamp01(5. * (tangent - .7)));
		}

		if (!stop) {
			deltavel *= .1;
		}

		if (InputManager::GetKey(KeyCode::Space)) {
			if (grounded) vel.y = 10;
		}

		playerBody->SetVelocity(vel + deltavel);

		//cam rotation
		auto delta = InputManager::GetMouseDelta();

		delta = delta * .2;

		pos += delta;
		pos.y = Math::Clamp(pos.y, -90., 90.);
		pos.x = fmod(pos.x, 360);
		cam->rotation = Quaternion::FromAxisAngle(yrot * Vector3::right, pos.y) * yrot;
		cam->position = transform->position + Vector3::up;

		if (InputManager::GetKey(KeyCode::MouseLeft)) {
			InputManager::LockMouse();
			EraseVoxels();
		} else if (InputManager::GetKey(KeyCode::MouseRight)) {
			AddVoxels();
		}
	}

	bool updating = false;
	long timeout = 100;

	void AddVoxels() {
		auto physEn = GetScene().GetPhysEngine();
		auto cam = GetRender().GetActiveCamera();
		if (!updating) {
			HitInfo info;
			if (physEn->Raycast(cam->position, cam->rotation * Vector3::forward, info, 128, .1, 5.)) {
				info.pos -= info.norm * 1.5;
				SDFunc func = [info](Vector3 p) {return sdSphere(p - info.pos, 2); };
				Vector3i minPos = info.pos - Vector3::one * 3;
				Core::RunTask([this, func, minPos]() {
					updating = true;
					auto time = steady_clock::now();
					chunkGen->world->AddVoxels(func, minPos, minPos + Vector3i(6, 6, 6), VoxelTypeIndex::V_SAND);
					std::this_thread::sleep_until(time + milliseconds(timeout));
					updating = false;
					});
			}
		}
	}

	void EraseVoxels() {
		auto physEn = GetScene().GetPhysEngine();
		auto cam = GetRender().GetActiveCamera();
		if (!updating) {
			HitInfo info;
			if (physEn->Raycast(cam->position, cam->rotation * Vector3::forward, info, 128, .1, 5.)) {
				info.pos += info.norm * 1.4;
				SDFunc func = [info](Vector3 p) { return sdSphere(p - info.pos, 2); };
				Vector3i minPos = info.pos - Vector3::one * 5;
				Core::RunTask([this, func, minPos]() {
					updating = true;
					auto time = steady_clock::now();
					chunkGen->world->EraseVoxels(func, minPos, minPos + Vector3i(10, 10, 10));
					std::this_thread::sleep_until(time + milliseconds(timeout));
					updating = false;
					});
			}
		}
	}
};
