#pragma once
#include "IRigidBody.h"
#include "../Util/SmartPointer.h"
#include <unordered_map>
#include <unordered_set>
#include "../Vectors.h"
#include "../Vectors/Hasher.h"
namespace Game::Physics {
	class PhysicsEngine {
	private:
		struct Frame {
		private:
			struct Data {
				Vector3 pos, velo;
			};
			std::unordered_map<SmartPtr<IRigidbody>, Data> rbData;
		public:
			const Vector3 GetPosition(SmartPtr<IRigidbody>) const;
			void SetPosition(SmartPtr<IRigidbody>, Vector3);
			void SetVelocity(SmartPtr<IRigidbody>, Vector3);
		};
	private:
		float m_tps = 25;
		Frame frontFrame, backFrame;
		std::unordered_set<SmartPtr<IRigidbody>> bodies{};
		std::unordered_set<SmartPtr<ICollider>> colliders{};
		std::mutex swapMutex, updateMutex;
		SmartPtr<std::thread> updateThread;
		bool alive;
	public:
		void SetTPS(float tps) { m_tps = tps; }
		float GetTPS() { return m_tps; }
		void AddRigidBody(SmartPtr<IRigidbody> rigidbody);
		void RemoveRigidBody(SmartPtr<IRigidbody> rigidbody);
		void AddCollider(SmartPtr<ICollider> collider);
		void RemoveCollider(SmartPtr<ICollider> collider);
		void Init();
		void Update(dbl dt);
		const Frame GetBackFrame();
		~PhysicsEngine() { alive = false; }
	private:
		dbl sdFunc(Vector3 pos);
		void SwapFrames();
		void CaptureFrame();
	};
}