#pragma once
#include "../Vectors/Vector3.h"
#include "ICollider.h"

namespace Game::Physics {
	__interface IRigidbody : public ICollider {
		Vector3 Move(dbl deltaTime, std::function<dbl(Vector3)> sdFunc);
		Vector3 GetVelocity();
		Vector3 GetPosition();
	};
}