#include "pch.h"
#include <GameEngine\Components\Rigidbody.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	using namespace Physics;

	Vector3 Rigidbody::Move(dbl deltaTime, std::function<dbl(Vector3)> sdFunc) {
		std::lock_guard lock(engineMutex);
		auto motion = velocity * deltaTime;

		dbl d0 = sdFunc(position);

		auto motionL = motion.Length();

		if (d0 < motionL) {
			motion *= d0 / motionL;
		}

		auto newPos = position + motion;

		dbl d = sdFunc(newPos);

		dbl delta = radius - d;

		if (delta > 0) {
			constexpr dbl ep = 0.1;
			constexpr dbl re = 10.;
			Vector3 n = {
				sdFunc(newPos + Vector3::right * ep) - d,
				sdFunc(newPos + Vector3::up * ep) - d,
				sdFunc(newPos + Vector3::forward * ep) - d };
			n *= re;
			n.Normalize();
			auto d = Vector3::Dot(velocity, n);
			//auto l = velocity.Length();
			velocity -= n * d * (1. + bounciness);
			/*if (abs(d) < l * .11) {
				velocity.Normalize();
				velocity *= l;
			}*/
			motion += n * delta;
			newPos = position + motion;
		}
		velocity += gravity * deltaTime;
		position = newPos;
		return motion;
	}

	Vector3 Rigidbody::GetVelocity() {
		std::lock_guard lock(engineMutex);
		return velocity;
	}

	Vector3 Rigidbody::GetPosition() {
		std::lock_guard lock(engineMutex);
		return position;
	}
	dbl Rigidbody::sdFunc(Vector3 p) {
		return (p - position).Length() - radius;
	}
}