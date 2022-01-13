#pragma once
#include "IRigidBody.h"
#include "../Util/SmartPointer.h"
#include <unordered_map>
#include <unordered_set>
#include "../Vectors.h"
#include "../Vectors/Hasher.h"
#include "HitInfo.h"

#define PHYS_FIXED_STEP 8

namespace Game::Physics {
	class PhysicsEngine {
	private:
		struct Frame {
		private:
			struct Data {
				Vector3 pos, prevPos, velo;
			};
			std::unordered_map<size_t, Data> rbData;
			std::chrono::steady_clock::time_point timePoint;
			dbl deltaTime;
		public:
			const Vector3 GetPosition(size_t UUID) const;
			Vector3 GetRawPosition(size_t UUID);
			void SetPosition(size_t, Vector3);
			void SetPrevPosition(size_t, Vector3);
			void SetVelocity(size_t, Vector3);
			void SetTime() { timePoint = std::chrono::steady_clock::now(); }
			void SetDeltaTime(dbl dt) { deltaTime = dt; }
		};
	private:
		size_t lastUUID = 0;
		float m_tps = 20;
		Frame frontFrame, backFrame;
		std::unordered_map<size_t, SmartPtr<IRigidbody>> bodies{};
		std::unordered_map<size_t, SmartPtr<ICollider>> colliders{};
		std::mutex swapMutex, updateMutex;
		SmartPtr<std::thread> updateThread;
		bool alive = false;
		bool init = false;
		float timeScale = 1;
	public:
		void SetTPS(float tps) { m_tps = tps; }
		float GetTPS() { return m_tps; }
		size_t AddRigidBody(IRigidbody* rigidbody);
		void RemoveRigidBody(size_t UUID);
		size_t AddCollider(ICollider* collider);
		void RemoveCollider(size_t UUID);
		void Init();
		void Update(dbl dt);

		const Frame GetBackFrame();
		bool Raycast(Vector3 origin, Vector3 direction, OUT HitInfo& info, size_t maxIterations = 128, dbl eps = .1, dbl maxDist = 300.);

		std::mutex& GetUpdateMutex() { return updateMutex; }
		~PhysicsEngine() { alive = false; }
	private:
		dbl sdFunc(Vector3 pos);
		void SwapFrames();
		void CaptureFrame(dbl deltaTime);
	};
}