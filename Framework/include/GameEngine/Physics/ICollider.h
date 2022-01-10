#pragma once
#include "../Vectors/Vector3.h"

namespace Game::Physics {
	__interface ICollider {
		dbl sdFunc(Vector3 pos);
	};
}
