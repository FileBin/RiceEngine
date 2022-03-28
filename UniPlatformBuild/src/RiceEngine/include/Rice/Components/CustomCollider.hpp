#pragma once

#include "Collider.h"
#include "../Model.h"

namespace Game {
	class CustomCollider : public Collider {
	private:
		SDFunc func = [](Vector3) { return DBL_MAX; };
	public:
		CustomCollider() : Collider() {};
		void SetFunc(SDFunc f) { func = f; }
		dbl sdFunc(Vector3 p) { return func(p); }
	};
}