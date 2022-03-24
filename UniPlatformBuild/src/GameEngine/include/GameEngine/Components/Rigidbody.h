#pragma once

#include "../Scene/Component.h"
#include "../Physics/IRigidBody.h"
#include "../Physics/PhysicsEngine.h"
#include "../Util/SmartPointer.h"
#include "Transform.h"

namespace Game {
	using namespace Physics;
	class Rigidbody : public Component, public IRigidbody {
	private:
		float mass = 1.;

		Vector3 velocity;
		Vector3 rotationVelocity;
		Vector3 position;
		Vector3 gravity = { 0, -9.807, 0 };
		SmartPtr<Transform> transform;
		SmartPtr<PhysicsEngine> engine;
		size_t UUID = -1;

		dbl bounciness = 0;
		bool isCollided = false;

		std::shared_mutex engineMutex;

		//temporary
		dbl radius = 1;
	public:
		void OnEnable();
		void Update();
		void OnDisable();
		bool IsCollided() { return isCollided; }
	private:
		Vector3 Move(dbl deltaTime, std::function<dbl(Vector3)> sdFunc);
	public:
		void SetVelocity(Vector3 velocity);
		Vector3 GetVelocity();
		Vector3 GetPosition();
		dbl sdFunc(Vector3 p);

		Vector3 GetNormal(Vector3 p, dbl);
	};
}