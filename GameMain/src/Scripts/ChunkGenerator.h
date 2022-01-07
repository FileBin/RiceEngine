#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <Scripts\World\Chunk.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Components\Transform.h>

#include "World\World.h"
#include "World\FlatGenerator.h"
#include "World\StandartGenerator.h"

#include <thread>
#include <concurrent_priority_queue.h>

#include <ppltasks.h>
#include <queue>

using namespace std::chrono;
using namespace Game;
using std::thread, std::vector, concurrency::concurrent_priority_queue, concurrency::task;

class MyComparer {
public:
	bool operator()(Vector3i a, Vector3i b) {
		return (a.x * a.x + a.y * a.y + a.z * a.z) > (b.x * b.x + b.y * b.y + b.z * b.z);
	}
};

struct PooledChunk {
	SceneObject* obj = nullptr;
	Vector3i pos = {0,0,0};
	int lod = -1;
	bool busy = false;
};

class ChunkGenerator : public MonoScript {
	vector<bool> loading{};
	//Chunk* chunk;
	int renderDistance = 8;
	World* world;
	WorldGenerator* generator;

	vector<SmartPtr<thread>> threads;
	SmartPtr<thread> chunkLoaderThread{nullptr};
	concurrent_vector<PooledChunk> chunksPool{};

	Vector3 playerPos{0, 0, 0};

	size_t nLodThreads = 4;

	int lodDistances[5] = { 0, 3, 4, 5, 7 };

	void Start() {
#ifdef _DEBUG
		renderDistance = 3;
#endif // _DEBUG

		auto& scene = GetScene();

		playerPos.y = 30;

		//generator = new FlatGenerator(25.);
		generator = new StandartGenerator(WorldSeed::Default(), 60, -20, .3);
		world = new World(generator, &scene.GetRender());

		auto a = renderDistance * 2 + 1;

		size_t n = a * a * a;
		chunksPool.reserve(n);
		size_t poolSize = 0;
		for (int i = 0; i < n; i++) {
			Vector3i pos;
			auto idx = i;
			auto ad2 = a / 2;
			pos.x = idx % a - ad2;
			idx /= a;
			pos.y = idx % a - ad2;
			idx /= a;
			pos.z = idx - ad2;
			if (!CheckChunkVisible(pos)) continue;
			auto o = scene.Instaniate();
			auto render = new ModelRender();
			o->AddComponent(new Transform());
			o->AddComponent(render);
			chunksPool.push_back({ o, pos });
			for (auto i = poolSize; i > 0; i--) {
				if (chunksPool[i].pos.SqrLength() < chunksPool[i - 1].pos.SqrLength()) {
					std::swap(chunksPool[i], chunksPool[i - 1]);
				} else break;
			}
			poolSize++;
		}
		chunksPool.resize(poolSize);

		threads.resize(nLodThreads);

		for (size_t i = 0; i < nLodThreads; i++) {
			threads[i] = Core::RunThread<void(size_t)>([this](size_t i) { LodLoaderThread(i); }, i);
		}

		chunkLoaderThread = Core::RunThread<void()>([this]() { ChunkLoaderThread(); });
	}

	void Update() {
		auto& en = GetEngine();
		auto& ren = GetRender();
		auto waterMat = ren.GetMaterial(L"Water");
		waterMat->SetVar(L"time", (float)en.GetTime());
		auto res = Util::GetWindowScreenSize(en.GetHWND());
		waterMat->SetVar<Vector2f>(L"resolution", { (float)res.x, (float)res.y });
		waterMat->UpdateBuffer();

		auto newPos = ren.GetActiveCamera()->position;
		auto playerChunk = World::TransformToChunkPos(newPos);
		if (playerChunk != World::TransformToChunkPos(playerPos)) {
			playerPos = newPos;
			Log::log(Log::INFO, L"ChunkStatus: {}", (int)world->GetChunkStatus(playerChunk));
		}
	}

	Vector3i playerChunk;

	void LodLoaderThread(size_t lodIdx) {
		int thIdx = lodIdx + 1;
		auto& sRen = GetScene().GetRender();
		auto poolSize = chunksPool.size();
		auto minD = lodDistances[lodIdx];
		auto maxD = lodDistances[lodIdx + 1];
		minD *= minD;
		maxD *= maxD;
		while (enabled) {
			for (size_t i = 0; i < poolSize; i++) {
				if (!enabled) return;
				auto& pooledCh = chunksPool.at(i);
				if (pooledCh.busy) continue;
				pooledCh.busy = true;
				if (pooledCh.obj->isEnabled()) {
					auto locPos = pooledCh.pos - playerChunk;
					auto sqrlen = locPos.SqrLength();
					if (sqrlen < minD || sqrlen >= maxD) {
						pooledCh.busy = false;
						continue;
					}
					if (CheckChunkVisible(locPos)) {
						int lod = lodIdx;
#ifdef _DEBUG
						lod++;
#endif // _DEBUG

						//lod = Math::Clamp(lod, 0, 3);
						if (lod != pooledCh.lod) {
							auto model = world->GetChunk(pooledCh.pos)->GetModel(lod);
							auto render = pooledCh.obj->GetComponents<ModelRender>()[0];
							sRen.WaitRendering();
							sRen.Lock(thIdx);
							render->SetModel(model);
							pooledCh.lod = lod;
							sRen.Unlock(thIdx);
						}
					}
				}
				pooledCh.busy = false;
			}
			Sleep(10);
		}
	}


	void ChunkLoaderThread() {
		int thIdx = nLodThreads + 1;
		auto& sRen = GetScene().GetRender();
		auto poolSize = chunksPool.size();

		std::vector<Vector3i> positions;
		positions.reserve(poolSize);

		std::queue<Vector3i> toLoad;
		for (auto i = 0; i < poolSize; i++) {
			positions.push_back(chunksPool[i].pos);
		}

		playerChunk = World::TransformToChunkPos(playerPos);

		auto nPos = positions.size();
		for (auto i = 0; i < nPos; i++) {
			toLoad.push(chunksPool[i].pos + playerChunk);
		}
		auto nLoad = toLoad.size();

		unordered_map<Vector3i, bool> posStates{};

		while (enabled) {

			int lod = Chunk::GetMaxLod();

			auto newPlayerChunk = World::TransformToChunkPos(playerPos);
			if (playerChunk != newPlayerChunk) {
				int addedRadius = ceil((playerChunk - newPlayerChunk).Length());
				world->UnloadChunks([&](Vector3i wpos) {
					return CheckChunkVisible(wpos - newPlayerChunk, addedRadius);
					});
				playerChunk = newPlayerChunk;
				while (!toLoad.empty()) toLoad.pop();
				for (auto j = 0; j < nPos; j++) {
					bool f = false;

					auto wPos = positions[j] + playerChunk;

					if (posStates.find(wPos) == posStates.end()) {
						toLoad.push(wPos);
					}
				}
			}

			for (auto i = 0; i < nLoad; i++) {
				if (!enabled) return;
				auto poolIdx = i;
				auto& pooledCh = chunksPool.at(poolIdx);
				if (pooledCh.busy) continue;
				pooledCh.busy = true;
				auto render = pooledCh.obj->GetComponents<ModelRender>()[0];
				auto transform = pooledCh.obj->GetComponents<Transform>()[0];
				if (!pooledCh.obj->isEnabled()) {
					if (toLoad.empty()) {
						pooledCh.busy = false;
						continue; 
					}
					auto& chPos = toLoad.front();
					auto ch = world->GetChunk(chPos);
					toLoad.pop();
					auto model = ch->GetModel(lod);
					sRen.WaitRendering();
					sRen.Lock(thIdx);
					pooledCh.pos = chPos;
					transform->position = World::TransformToWorldPos(chPos);
					render->SetModel(model);
					for (auto i = 0; i < Voxel::GetMaterialCount(); i++) {
						render->SetMaterial(SmartPtr<Material>(Voxel::GetMaterialAt(i)), i);
					}
					pooledCh.obj->Enable();
					posStates.insert({ pooledCh.pos, true });
					sRen.Unlock(thIdx);
				} else {
					auto locPos = pooledCh.pos - playerChunk;
					if (CheckChunkVisible(locPos)) {
						/*int lod = floor(locPos.Length()) - 1;
#ifdef _DEBUG
						lod++;
#endif // _DEBUG

						lod = Math::Clamp(lod, 0, 3);
						if (lod != pooledCh.lod) {
							auto model = world->GetChunk(pooledCh.pos).GetModel(lod);
							sRen.WaitRendering();
							sRen.Lock(thIdx);
							render->SetModel(model);
							pooledCh.lod = lod;
							sRen.Unlock(thIdx);
						}*/
					} else {
						auto it = posStates.find(pooledCh.pos);
						if (it != posStates.end())
							posStates.erase(it);
						sRen.WaitRendering();
						sRen.Lock(thIdx);
						pooledCh.obj->Disable();
						render->DeleteModel();
						sRen.Unlock(thIdx);
						//concurrency::create_task([&]() { world->UnloadChunk(pooledCh.pos); });
						//world->UnloadChunk(pooledCh.pos);
					}

				}
				pooledCh.busy = false;
			}
			Sleep(10);
		}
	}

	bool CheckChunkVisible(Vector3i chunkPos, int addedDistance = 0) {
		return chunkPos.Length() <= renderDistance + addedDistance;
	}

	void OnDisable() {
		enabled = false;
		world->Unlock();
		chunkLoaderThread->join();
		chunkLoaderThread.Release();
		for (auto& t : threads) {
			t->join();
			t.Release();
		}
	}
};