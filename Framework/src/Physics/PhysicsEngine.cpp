#include "pch.h"
#include <GameEngine\Physics\PhysicsEngine.h>
#include <float.h>
#include <GameEngine\Math.h>
#include <GameEngine\Core.h>
namespace Game::Physics {

	void PhysicsEngine::AddRigidBody(SmartPtr<IRigidbody> rigidbody) { std::lock_guard lock(updateMutex); bodies.insert(rigidbody); }
	void PhysicsEngine::RemoveRigidBody(SmartPtr<IRigidbody> rigidbody) { std::lock_guard lock(updateMutex); bodies.erase(rigidbody); }
	void PhysicsEngine::AddCollider(SmartPtr<ICollider> collider) { std::lock_guard lock(updateMutex); colliders.insert(collider); }
	void PhysicsEngine::RemoveCollider(SmartPtr<ICollider> collider) { std::lock_guard lock(updateMutex); colliders.erase(collider); }

	void PhysicsEngine::Init() {
		using namespace std::chrono;
		alive = true;
		updateThread = Core::RunThread<void(void)>([this]() {
			dbl fixedDeltaTime = 1 / m_tps;
			dbl deltaTime = fixedDeltaTime;
			long interval = fixedDeltaTime*1000.;
			while (alive) {
				auto frameStart = steady_clock::now();
				Update(deltaTime);
				std::this_thread::sleep_until(frameStart + milliseconds(interval));
				deltaTime = .001 * .000001 * (steady_clock::now() - frameStart).count();
			}
			});
	}

	void PhysicsEngine::Update(dbl dt) {
		std::lock_guard lock(updateMutex);
		for (auto& rb : bodies) {
			rb->Move(rb->GetVelocity() * dt, [this](Vector3 pos) { return sdFunc(pos); });
		}
		CaptureFrame();
		SwapFrames();
	}

	const PhysicsEngine::Frame PhysicsEngine::GetBackFrame() {
		std::lock_guard lock(swapMutex);
		return backFrame;
	}

	dbl PhysicsEngine::sdFunc(Vector3 pos) {
		auto d = DBL_MAX;
		for (auto& col : colliders) {
			d = Math::Min(d, col->sdFunc(pos));
		}
		for (auto& col : bodies) {
			d = Math::Min(d, col->sdFunc(pos));
		}
		return d;
	}

	void PhysicsEngine::SwapFrames() {
		std::lock_guard lock(swapMutex);
		auto temp = backFrame;
		backFrame = frontFrame;
		frontFrame = temp;
	}

	void PhysicsEngine::CaptureFrame() {
		for (auto& rb : bodies) {
			backFrame.SetPosition(rb, rb->GetPosition());
			backFrame.SetVelocity(rb, rb->GetVelocity());
		}
	}

	const Vector3 PhysicsEngine::Frame::GetPosition(SmartPtr<IRigidbody> rb) const {
		try {
			return rbData.at(rb).pos;
		} catch (std::out_of_range) {
			THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
		}
	}

	void PhysicsEngine::Frame::SetPosition(SmartPtr<IRigidbody> rb, Vector3 pos) { rbData[rb].pos = pos; }
	void PhysicsEngine::Frame::SetVelocity(SmartPtr<IRigidbody> rb, Vector3 velo) { rbData[rb].velo = velo; }
}