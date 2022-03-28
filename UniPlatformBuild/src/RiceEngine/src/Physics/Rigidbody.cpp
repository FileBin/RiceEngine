#include "pch.h"
#include <GameEngine\Components\Rigidbody.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	using namespace Physics;

	constexpr size_t MaxIterations = 2;

	Vector3 Rigidbody::Move(dbl deltaTime, std::function<dbl(Vector3)> sdFunc) {
		auto physEn = GetSceneObject().GetScene().GetPhysEngine();
		std::unique_lock lock(engineMutex);
		Vector3 dVelo{0,0,0};
		dbl veloIters = 0;
		velocity += gravity * deltaTime;
		auto motion = velocity * deltaTime;

		dbl d0 = sdFunc(position);

		auto motionL = motion.Length();

		if (d0 < motionL) {
			motion *= d0 / motionL;
		}

		auto newPos = position + motion;

		bool collided = false;

		for (size_t i = 0; i < MaxIterations; i++) {

			dbl d = sdFunc(newPos);

			dbl delta = radius - d;

			//if (delta >= -collisionDistance) {
			//	collided = true;
			//}

			if (delta > 0) {
				collided = true;
				Vector3 n = physEn->GetNormal(newPos, .005);
				motion += n * delta * .99;
				dbl dot = Vector3::Dot(velocity, n);
				veloIters++;
				velocity -= n * dot * (1. + bounciness);
				newPos = position + motion;
			} else {
				break;
			}
		}
		isCollided = collided;
		position = newPos;
		velocity += dVelo / max(1., veloIters);
		return motion;
	}

	Vector3 Rigidbody::GetVelocity() {
		std::shared_lock lock(engineMutex);
		return velocity;
	}

	Vector3 Rigidbody::GetPosition() {
		std::shared_lock lock(engineMutex);
		return position;
	}

	dbl Rigidbody::sdFunc(Vector3 p) {
		return (p - position).Length() - radius;
	}

	Vector3 Rigidbody::GetNormal(Vector3 p, dbl) {
		return (p - position).Normalized();
	}
}