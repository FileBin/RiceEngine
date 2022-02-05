#include "pch.h"
#include <GameEngine\Physics\PhysicsEngine.h>
#include <float.h>
#include <GameEngine\Math.h>
#include <GameEngine\Core.h>
namespace Game::Physics {

	size_t PhysicsEngine::AddRigidBody(IRigidbody* rigidbody) {
		std::unique_lock lock(updateMutex);
		bodies[lastUUID] = rigidbody;
		return lastUUID++;
	}
	void PhysicsEngine::RemoveRigidBody(size_t UUID) { 
		std::unique_lock lock(updateMutex);
		bodies.erase(UUID);
	}
	size_t PhysicsEngine::AddCollider(ICollider* collider) {
		std::unique_lock lock(updateMutex);
		colliders[lastUUID] = collider;
		return lastUUID++;
	}
	void PhysicsEngine::RemoveCollider(size_t UUID) {
		std::unique_lock lock(updateMutex);
		colliders.erase(UUID);
	}

	void PhysicsEngine::PreInit() {
		using namespace std::chrono;
		alive = true;
		updateThread = Core::RunThread<void(void)>([this]() {
			SetThreadPriority(GetCurrentThread(), 1);
			dbl fixedDeltaTime = 1 / m_tps;
			dbl deltaTime = fixedDeltaTime;
			long interval = fixedDeltaTime*1000. * 1000. - 10;
			while (alive) {
				auto frameStart = steady_clock::now();
				Update(deltaTime);
				std::this_thread::sleep_until(frameStart + microseconds(interval));
				deltaTime = .001 * .000001 * (steady_clock::now() - frameStart).count();
				deltaTime = Math::Min(deltaTime, fixedDeltaTime * 2);
				init = true;
			}
			});
	}

	void PhysicsEngine::Update(dbl dt) {
		std::unique_lock lock(updateMutex);
		for (const auto& p : bodies) {
			auto rb = p.second;
			rb->Move(timeScale * dt, [this](Vector3 pos) { return sdFunc(pos); });
		}
		CaptureFrame(dt);
		SwapFrames();
	}

	const PhysicsEngine::Frame PhysicsEngine::GetBackFrame() {
		std::shared_lock lock(swapMutex);
		while (!init)
			Sleep(1);
		return backFrame;
	}

	dbl PhysicsEngine::sdFunc(Vector3 pos) {
		auto d = std::numeric_limits<dbl>().infinity();
		for (const auto& p : colliders) {
			d = Math::Min(d, p.second->sdFunc(pos));
		}
		/*for (const auto& p : bodies) {
			if (p.first == idx) continue;
			d = Math::Min(d, p.second->sdFunc(pos));
		}*/
		return d;
	}

	/*Vector3 PhysicsEngine::GetNormal(Vector3 pos) {
		SmartPtr<ICollider> col;
		auto d = DBL_MAX;
		for (const auto& p : colliders) {
			auto _d = p.second->sdFunc(pos);
			if (d > _d) {
				d = _d;
				col = p.second;
			}
		}

		return col->GetNormal(pos);
	}*/
	void PhysicsEngine::SwapFrames() {
		std::unique_lock lock(swapMutex);
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
			frontFrame.SetPrevPosition(p.first, backFrame.GetRawPosition(p.first));
			frontFrame.SetVelocity(p.first, rb->GetVelocity() * (dbl)timeScale);
		}
	}

	bool PhysicsEngine::Raycast(Vector3 origin, Vector3 dir, OUT HitInfo& info, size_t maxIters, dbl eps, dbl maxD) {
		Vector3 pos = origin;
		dbl dist = 0;
		for (size_t i = 0; i < maxIters; i++) {
			updateMutex.lock();
			auto d = sdFunc(pos);
			updateMutex.unlock();
			if (d >= DBL_MAX) d = PHYS_FIXED_STEP;
			dist += d;
			if (d <= eps) {
				info.pos = pos + dir * d;
				info.dist = d;
				updateMutex.lock();
				info.norm = Math::GetNorm([this](Vector3 p) { return sdFunc(p); }, pos - dir * .1);
				updateMutex.unlock();
				return true;
			} else if (dist >= maxD) {
				break;
			}
			pos += dir * d;
		}
		return false;
	}

	const Vector3 PhysicsEngine::Frame::GetPosition(size_t uuid) const {
		try {
			using namespace std::chrono;
			const auto& data = rbData.at(uuid);
			auto k = (steady_clock::now() - timePoint).count() * .000001 * .001;
			k /= deltaTime;
			k = Math::Clamp01(k);

			return Vector3::Lerp(data.prevPos, data.pos, k);
		} catch (std::out_of_range) {
			THROW_INDEX_OUT_OF_RANGE_EXCEPTION;
		}
	}

	Vector3 PhysicsEngine::Frame::GetRawPosition(size_t uuid) {
		return rbData[uuid].pos;
	}

	void PhysicsEngine::Frame::SetPosition(size_t uuid, Vector3 pos) {
		rbData[uuid].pos = pos;
	}

	void PhysicsEngine::Frame::SetPrevPosition(size_t uuid, Vector3 pos) {
		rbData[uuid].prevPos = pos;
	}
	void PhysicsEngine::Frame::SetVelocity(size_t uuid, Vector3 velo) { rbData[uuid].velo = velo; }
}