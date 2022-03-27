#pragma once

#include "../Scene/Scene.h"
#include "../Physics/ICollider.h"
#include <float.h>
#include "Transform.h"

namespace Game {
	class Collider : public Component, public Physics::ICollider {
	protected:
		SmartPtr<Transform> transform;
		SmartPtr<Physics::PhysicsEngine> engine;
		size_t UUID = SIZE_T_MAX;
		//std::function<dbl(Vector3)> sd = [](Vector3 v) { return DBL_MAX; };
	public:
		Collider() = default;
		//Collider(std::function<dbl(Vector3)> sd_func);
		void OnEnable();
		void OnDisable();
	protected:
		virtual dbl sdFunc(Vector3 p) = 0;
	};
}