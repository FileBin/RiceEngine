#include "pch.h"
#include <GameEngine\Physics\PhysicsEngine.h>
#include <float.h>
#include <GameEngine\Math.h>
#include <GameEngine\Core.h>
namespace Game::Physics {

	size_t PhysicsEngine::AddRigidBody(IRigidbody* rigidbody) {
		std::lock_guard lock(updateMutex);
		bodies[lastUUID] = rigidbody;
		return lastUUID++;
	}
	void PhysicsEngine::RemoveRigidBody(size_t UUID) { 
		std::lock_guard lock(updateMutex);
		bodies.erase(UUID);
	}
	size_t PhysicsEngine::AddCollider(ICollider* collider) {
		std::lock_guard lock(updateMutex);
		colliders[lastUUID] = collider;
		return lastUUID++;
	}
	void PhysicsEngine::RemoveCollider(size_t UUID) {
		std::lock_guard lock(updateMutex);
		colliders.erase(UUID);
	}

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
				init = true;
			}
			});
	}

	void PhysicsEngine::Update(dbl dt) {
		std::lock_guard lock(updateMutex);
		for (const auto& p : bodies) {
			auto rb = p.second;
			rb->Move(timeScale * dt, [this, p](Vector3 pos) { return sdFunc(pos, p.first); });
		}
		CaptureFrame(dt);
		SwapFrames();
	}

	const PhysicsEngine::Frame PhysicsEngine::GetBackFrame() {
		std::lock_guard lock(swapMutex);
		while (!init)
			Sleep(1);
		return backFrame;
	}

	dbl PhysicsEngine::sdFunc(Vector3 pos, size_t idx) {
		auto d = DBL_MAX;
		for (const auto& p : colliders) {
			if (p.first == idx) continue;
			d = Math::Min(d, p.second->sdFunc(pos));
		}
		for (const auto& p : bodies) {
			if (p.first == idx) continue;
			d = Math::Min(d, p.second->sdFunc(pos));
		}
		return d;
	}

	void PhysicsEngine::SwapFrames() {
		std::lock_guard lock(swapMutex);
		auto temp = backFrame;
		backFrame = frontFrame;
		frontFrame = temp;
	}

	void PhysicsEngine::CaptureFrame(dbl deltaTime) {
		frontFrame.SetTime();
		frontFrame.SetDeltaTime(deltaTime);
		for (const auto& p : bodies) {
			auto rb = p.second;
			frontFrame.SetPosition(p.first, rb->GetPosition());
			frontFrame.SetVelocity(p.first, rb->GetVelocity() * (dbl)timeScale);
		}
	}

	const Vector3 PhysicsEngine::Frame::GetPosition(size_t uuid) const {
		try {
			using namespace std::chrono;
			const auto& data = rbData.at(uuid);
			auto k = (steady_clock::now() - timePoint).count() * .000001 * .001;
			return data.pos + data.velo * k;
		} catch (std::out_of_range) {
			THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
		}
	}

	void PhysicsEngine::Frame::SetPosition(size_t uuid, Vector3 pos) { rbData[uuid].pos = pos; }
	void PhysicsEngine::Frame::SetVelocity(size_t uuid, Vector3 velo) { rbData[uuid].velo = velo; }
}