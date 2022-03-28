#include "pch.h"
#include <GameEngine\Components\Collider.h>

namespace Game {

	/*Collider::Collider(std::function<dbl(Vector3)> sd_func) {
		sd = sd_func;
	}*/

	void Collider::OnEnable() {
		auto& obj = GetSceneObject();
		auto& scene = obj.GetScene();
		engine = scene.GetPhysEngine();
		UUID = engine->AddCollider(this);
		transform = obj.GetComponents<Transform>()[0];
	}

	void Collider::OnDisable() {
  		engine->RemoveCollider(UUID);
	}
}