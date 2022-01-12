#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <Scripts\World\Chunk.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Components\Transform.h>
#include <GameEngine\Components\MeshCollider.h>

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
	std::mutex* busy = nullptr;

	void Swap(PooledChunk& other) {
		std::swap(obj, other.obj);
		std::swap(pos, other.pos);
		std::swap(lod, other.lod);
		auto p = busy;
		busy = other.busy;
		other.busy = p;
	}

	~PooledChunk() {
		if (busy) {
			busy->lock();
			delete busy;
		}
	}
};

class ChunkGenerator : public MonoScript {
	vector<bool> loading{};
	//Chunk* chunk;
	int renderDistance = 10;
	World* world;
	WorldGenerator* generator;

	vector<SmartPtr<thread>> threads;
	SmartPtr<thread> chunkLoaderThread{nullptr};
	concurrent_vector<PooledChunk> chunksPool{};

	Vector3 playerPos{0, 0, 0};

	size_t nLodThreads = 4;

	int lodDistances[4] = { 2, 4, 5, 6 };

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
			o->AddComponent(new MeshCollider());
			o->AddComponent(render);
			chunksPool.push_back({ o, pos, 10000, nullptr });
			chunksPool[poolSize].busy = new std::mutex();
			poolSize++;
		}
		chunksPool.resize(poolSize);
		ReorderPool();
		/*for (auto& ch : chunksPool) {
			ch.busy = new std::mutex();
		}*/

		threads.resize(nLodThreads);
		for (size_t i = 0; i < nLodThreads; i++) {
			threads[i] = Core::RunThread<void(size_t,int, int)>([this](size_t lod, int idx, int r) { LodLoaderThread(lod, idx, r); }, i, i, lodDistances[i]);
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

	void LodLoaderThread(const size_t lodIdx, int thIdx, int radius) {
		int nSkips = 0;
		auto& sRen = GetScene().GetRender();
		auto poolSize = chunksPool.size();
		auto maxD = radius;
		maxD *= maxD;
		auto pPos = World::TransformToChunkPos(playerPos);
		while (enabled) {
			for (size_t i = 0; i < poolSize; i++) {
				if (!enabled) return;
				auto& pooledCh = chunksPool.at(i);
				if (!pooledCh.busy->try_lock()) { nSkips++; continue; }
				if (pooledCh.obj->isEnabled()) {
					auto locPos = pooledCh.pos - World::TransformToChunkPos(playerPos);
					auto sqrlen = locPos.SqrLength();
					//if (sqrlen - maxD == 0) {
						//if (rand() % 3 == 0) continue;
					//}
					if (sqrlen >= maxD) {
						pooledCh.busy->unlock();
						continue;
					}
					//if (Skip(pooledCh.pos)) { pooledCh.busy->unlock(); nSkips++; continue; }
					if (CheckChunkVisible(locPos)) {
						int lod = lodIdx;
#ifdef _DEBUG
						lod++;
#endif // _DEBUG

						//lod = Math::Clamp(lod, 0, 3);
						if (lod < pooledCh.lod) {
							auto model = world->GetChunk(pooledCh.pos)->GetModel(lod);
							auto render = pooledCh.obj->GetComponents<ModelRender>()[0];
							auto collider = pooledCh.obj->GetComponents<MeshCollider>()[0];
							collider->Disable();
							collider->SetModel(model.Get());
							collider->Enable();
							render->SetModel(model);
							if (lod == 0) {

							}
							pooledCh.lod = lod;
						}
					}
				}
				pooledCh.busy->unlock();
			}
			auto newPos = World::TransformToChunkPos(playerPos);
			if (pPos == newPos) {
				if (nSkips <= 0)
					Sleep(200);
			} else {
				pPos = newPos;
				Sleep(20 * lodIdx);
			}
			nSkips = 0;
		}
	}


	void ChunkLoaderThread() {
		int nSkips = 0;
		int thIdx = nLodThreads + 10;
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
				//ReorderPool();
				nSkips++;
				int addedRadius = ceil((playerChunk - newPlayerChunk).Length());
				world->UnloadChunks([&](Vector3i wpos) {
					return !CheckChunkVisible(wpos - newPlayerChunk, addedRadius);
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
				if (!pooledCh.busy->try_lock()) { nSkips++; continue; }
				auto render = pooledCh.obj->GetComponents<ModelRender>()[0];
				auto transform = pooledCh.obj->GetComponents<Transform>()[0];
				if (!pooledCh.obj->isEnabled()) {
					if (toLoad.empty()) {
						pooledCh.busy->unlock();
						continue; 
					}
					auto& chPos = toLoad.front();
					auto ch = world->GetChunk(chPos);
					toLoad.pop();
					auto model = ch->GetModel(lod);
					pooledCh.lod = lod;
					//sRen.WaitRendering();
					pooledCh.pos = chPos;
					transform->position = World::TransformToWorldPos(chPos);
					render->SetModel(model);
					for (auto i = 0; i < Voxel::GetMaterialCount(); i++) {
						render->SetMaterial(SmartPtr<Material>(Voxel::GetMaterialAt(i)), i);
					}
					pooledCh.obj->Enable();
					posStates.insert({ pooledCh.pos, true });
				} else {
					auto locPos = pooledCh.pos - playerChunk;
					if (!CheckChunkVisible(locPos)) {
						auto it = posStates.find(pooledCh.pos);
						if (it != posStates.end())
							posStates.erase(it);
						//sRen.WaitRendering();
						pooledCh.obj->Disable();
						//render->DeleteModel();
						//concurrency::create_task([&]() { world->UnloadChunk(pooledCh.pos); });
						//world->UnloadChunk(pooledCh.pos);
					}

				}
				pooledCh.busy->unlock();
			}
			if (nSkips <= 0) {
				Sleep(200);
			}
			nSkips = 0;
		}
	}

	bool CheckChunkVisible(Vector3i chunkPos, int addedDistance = 0) {
		return chunkPos.Length() <= renderDistance + addedDistance;
	}

	bool Skip(Vector3i wChunkPos) {
		int d = 1;
		auto wPos = World::TransformToWorldPos(wChunkPos) + Vector3::one * Chunk::ChunkSize/2;
		//d *= GetRender().GetActiveCamera()->ToScreenPos(wPos).Length() + 1;
		auto depth = world->GetChunkAltitude(wChunkPos);
		depth = Math::Clamp(-depth, 1, 4);
		d *= depth;
		return rand() % d != 0;
	}

	void ReorderPool() {
		auto n = chunksPool.size();
		for (auto j = 0; j < n; j++) {
			for (auto i = j; i > 0; i--) {
				if (chunksPool[i].pos.SqrLength() < chunksPool[i - 1].pos.SqrLength()) {
					chunksPool[i].Swap(chunksPool[i - 1]);
				} else break;
			}
		}
	}

	void OnDisable() {
		enabled = false;
		chunkLoaderThread->join();
		chunkLoaderThread.Release();
		for (auto& t : threads) {
			t->join();
			t.Release();
		}
	}
};