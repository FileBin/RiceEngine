#include "pch.h"

#include <GameEngine\Util\DistanceEstimator.h>
#include <GameEngine\Math.h>

namespace Game::Util {
	dbl opIntersect90deg(dbl a, dbl b) {
		auto m = max(a, b);
		if (m <= 0) return m;
		a = max(a, 0);
		b = max(b, 0);
		return sqrt(a * a + b * b);
	}

	dbl opIntersect(std::vector<dbl> distances) {
		dbl r = -DBL_MAX;
		size_t n = distances.size();
		for (size_t i = 0; i < n; i++) {
			r = max(r, distances[i]);
		}
		return r;
	}

	dbl opJoin(dbl a, dbl b) {
		return Math::Min(a, b);
	}

	dbl sdPlane(Vector3 p, Vector3 n) {
		return Vector3::Dot(p, n);
	}

	dbl sdLine(Vector3 p, Vector3 a, Vector3 b) {
		Vector3 pa = p - a, ba = b - a;
		dbl h = Math::Clamp01(Vector3::Dot(pa, ba) / Vector3::Dot(ba, ba));
		return (pa - ba * h).Length();
	}

	dbl sdSphere(Vector3 p, dbl r) {
		return p.Length() - r;
	}

	dbl sdTorus(Vector3 pos, dbl radius, dbl thickness) {
		Vector2 q = { sqrt(pos.x * pos.x + pos.z * pos.z) - radius, pos.y };
		return q.Length() - thickness;
	}
}