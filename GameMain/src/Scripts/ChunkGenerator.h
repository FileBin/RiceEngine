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
	int lod = 3;
	bool busy = false;
};

class ChunkGenerator : public MonoScript {
	vector<bool> loading{};
	//Chunk* chunk;
	int renderDistance = 10;
	World* world;
	WorldGenerator* generator;

	vector<thread*> threads;
	concurrent_vector<PooledChunk> chunksPool{};

	Vector3 playerPos{0, 0, 0};

	size_t nThreads = 2;

	void Start() {
		auto& scene = GetScene();

		playerPos.y = 30;

		//generator = new FlatGenerator(25.);
		generator = new StandartGenerator(WorldSeed::Default(), 60, -20, .3);
		world = new World(generator, &scene.GetRender());

		auto a = renderDistance * 2 + 1;

		size_t n = a * a * a;
		chunksPool.resize(n * nThreads);
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
			int lod = pos.Length() * .5;
			lod = Math::Min(lod, 3);
			if (!CheckChunkVisible(pos)) continue;
			auto o = scene.Instaniate();
			auto render = new ModelRender();
			o->AddComponent(new Transform());
			o->AddComponent(render);
			chunksPool[poolSize] = { o, pos, lod, false };
			for (auto i = poolSize; i > 0; i--) {
				if (chunksPool[i].pos.SqrLength() < chunksPool[i - 1].pos.SqrLength()) {
					std::swap(chunksPool[i], chunksPool[i - 1]);
				} else break;
			}
			poolSize++;
		}
		chunksPool.resize(poolSize);

		threads.resize(nThreads);
		loading.resize(nThreads);

		for (size_t i = 0; i < threads.size();i++) {
			threads[i] = new thread(&ChunkGenerator::ChunkLoaderThread, this, i);
		}
	}

	void Update() {
		auto& en = GetEngine();
		auto& ren = GetRender();
		auto waterMat = ren.GetMaterial(L"Water");
		waterMat.SetVar(L"time", (float)en.GetTime());
		auto res = Util::GetWindowScreenSize(en.GetHWND());
		waterMat.SetVar<Vector2f>(L"resolution", { (float)res.x, (float)res.y });
		waterMat.UpdateBuffer();

		auto newPos = ren.GetCamera(0).position;
		auto playerChunk = World::TransformToChunkPos(newPos);
		if (playerChunk != World::TransformToChunkPos(playerPos)) {
			playerPos = newPos;
			Log::log(Log::INFO, L"ChunkStatus: {}", (int)world->GetChunkStatus(playerChunk));
		}
	}


	void ChunkLoaderThread(size_t idx) {
		auto& sRen = GetScene().GetRender();
		auto poolSize = chunksPool.size();
		std::vector<Vector3i> positions;
		std::vector<int> lods{};
		std::queue<Vector3i> toLoad;
		for (auto i = 0; i < poolSize; i++) {
			positions.push_back(chunksPool[i].pos);
			lods.push_back(chunksPool[i].lod);
		}
		auto nPos = positions.size();
		for (auto i = idx; i < nPos; i+= nThreads) {
			toLoad.push(chunksPool[i].pos);
		}
		auto nLoad = toLoad.size();

		auto playerChunk = World::TransformToChunkPos(playerPos);

		while (enabled) {
			auto newPlayerChunk = World::TransformToChunkPos(playerPos);
			if (playerChunk != newPlayerChunk) {
				playerChunk = newPlayerChunk;
				while (!toLoad.empty()) toLoad.pop();
				for (auto j = idx; j < nPos; j += nThreads) {
					bool f = false;
					for (auto i = 0; i < nPos; i++) {
						if (positions[j] == chunksPool[i].pos - playerChunk) {
							f = true;
							break;
						}
					}
					if (!f)
						toLoad.push(positions[j] + playerChunk);
				}
			}
			for (auto i = 0; i < nLoad; i++) {
				if (!enabled) return;
				auto& pooledCh = chunksPool.at(i*nThreads + idx);
				if (pooledCh.busy) continue;
				auto render = pooledCh.obj->GetComponents<ModelRender>()[0];
				auto transform = pooledCh.obj->GetComponents<Transform>()[0];
				if (!pooledCh.obj->isEnabled()) {
					if (toLoad.empty()) continue;
					pooledCh.busy = true;
					auto chPos = toLoad.front();
					auto& ch = world->GetChunk(chPos);
					toLoad.pop();
					auto& model = *ch.GetModel(pooledCh.lod);
					sRen.WaitRendering();
					sRen.Lock();
					pooledCh.pos = chPos;
					transform->position = World::TransformToWorldPos(chPos);
					render->SetModel(&model);
					for (auto i = 0; i < Voxel::GetMaterialCount(); i++) {
						render->SetMaterial(&Voxel::GetMaterialAt(i), i);
					}
					pooledCh.obj->Enable();
					sRen.Unlock();
				} else {
					bool founded = false;
					for (auto j = 0; j < nPos; j++) {
						if (positions[j] == pooledCh.pos - playerChunk) {
							if (i*nThreads+idx == j) continue;
							founded = true;
							if (lods[j] != pooledCh.lod) {
								sRen.WaitRendering();
								sRen.Lock();
								auto& model = *world->GetChunk(pooledCh.pos).GetModel(lods[j]);
								render->SetModel(&model);
								pooledCh.lod = lods[j];
								sRen.Unlock();
							}

							break;
						}
					}
					if (!founded) {
						sRen.WaitRendering();
						sRen.Lock();
						pooledCh.obj->Disable();
						//world->UnloadChunk(pooledCh.pos);
						sRen.Unlock();
					}

				}
				pooledCh.busy = false;
			}
			Sleep(1);
		}
	}

	bool CheckChunkVisible(Vector3i chunkPos, int addedDistance = 0) {
		return chunkPos.Length() <= renderDistance + addedDistance;
	}

	void OnDisable() {
		enabled = false;
		world->Unlock();
		for (auto t : threads) {
			t->join();
			delete t;
		}
	}
};