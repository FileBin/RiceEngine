#include "pch.h"
#include <GameEngine\Components\Rigidbody.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	using namespace Physics;
	void Rigidbody::OnEnable() {
		auto& obj = GetSceneObject();
		auto& scene = obj.GetScene();
		engine = scene.GetPhysEngine();
		UUID = engine->AddRigidBody(this);
		transform = obj.GetComponents<Transform>()[0];
		position = transform->GetPosition();
	}
	void Rigidbody::Update() {
		try {
			const auto RunFrame = engine->GetBackFrame();
			transform->SetPosition(RunFrame.GetPosition(UUID));
		} catch (IndexOutOfRangeException) {

		}
	}
	void Rigidbody::OnDisable() {
		engine->RemoveRigidBody(UUID);
	}

	void Rigidbody::SetVelocity(Vector3 velo) {
		std::unique_lock lock(engineMutex);
		velocity = velo;
	}
}