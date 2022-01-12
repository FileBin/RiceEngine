#pragma once
#include "defines.h"
#include "../Vectors.h"

namespace Game::Util {
	dbl opIntersect90deg(dbl a, dbl b);
	dbl opIntersect(std::vector<dbl> distances);
	dbl opJoin(dbl a, dbl b);
	dbl sdPlane(Vector3 p, Vector3 n);
	dbl sdLine(Vector3 p, Vector3 a, Vector3 b);
	dbl sdTorus(Vector3 pos, dbl radius, dbl thickness);
}
