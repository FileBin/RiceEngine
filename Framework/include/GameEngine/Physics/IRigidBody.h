#pragma once
#include "../Vectors/Vector3.h"
#include "ICollider.h"

namespace Game::Physics {
	__interface IRigidbody : public ICollider {
		Vector3 Move(Vector3 motion, std::function<dbl(Vector3)> sdFunc);
		Vector3 GetVelocity();
		Vector3 GetPosition();
	};
}