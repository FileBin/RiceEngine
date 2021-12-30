#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <Scripts\World\Chunk.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>

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

class ChunkGenerator : public MonoScript {
	bool unloading = false;
	vector<bool> loading{};
	//Chunk* chunk;
	int renderDistance = 15;
	World* world;
	WorldGenerator* generator;

	vector<thread*> threads;
	concurrent_vector<std::pair<SceneObject*, bool>> chunksPool{};

	Vector3 playerPos{0, 0, 0};

	size_t nThreads = 8;

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
			if (!CheckChunkVisible(pos)) continue;
			for (size_t j = 0; j < nThreads; j++) {
				auto o = scene.Instaniate();
				auto render = new ModelRender();
				o->AddComponent(render);
				chunksPool[poolSize].first = o;
				chunksPool[poolSize].second = false;
				poolSize++;
			}
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
		waterMat.UpdateBuffer();

		auto newPos = ren.GetCamera(0).transform.pos;
		auto playerChunk = World::TransformToChunkPos(newPos);
		if (playerChunk != World::TransformToChunkPos(playerPos)) {
			if (!unloading) {
				create_task([this]() { UnloadChunks(); });
			}
			Log::log(Log::INFO, L"ChunkStatus: {}", (int)world->GetChunkStatus(playerChunk));
		}
		playerPos = newPos;
	}

	Vector3i GetNextPosition(Vector3i pos) {
		if (pos.x == 0 && pos.z == 0) {
			if (pos.y == 0)
				return { 0,-1,0 };
			if (pos.y > 0) {
				pos.y = -pos.y - 1;
				return pos;
			}
			pos.z += -1;
			pos.y += 1;
			return pos;
		}
		if (pos.x >= 0 && pos.z < 0) {
			pos.x += 1;
			pos.z += 1;
			return pos;
		}
		if (pos.x > 0 && pos.z >= 0) {
			pos.x += -1;
			pos.z += 1;
			return pos;
		}
		if (pos.x <= 0 && pos.z > 0) {
			pos.x += -1;
			pos.z += -1;
			return pos;
		}
		if (pos.x < 0 && pos.z <= 0) {
			pos.x += 1;
			if (pos.x == 0) {
				if (pos.y < 0)
					pos.z += -2;
				pos.y += 1;
				return pos;
			}
			pos.z += -1;
			return pos;
		}
		return pos;
	}

	void ChunkLoaderThread(size_t idx) {
		Vector3i offset = {0,0,0};

		if (idx % 2 == 1) {
			offset.x = 1;
		}
		idx /= 2;
		if (idx % 2 == 1) {
			offset.z = 1;
		}
		idx /= 2;
		if (idx % 2 == 1) {
			offset.y = 1;
		}

		auto& o = GetSceneObject();
		auto& scene = GetScene();
		auto& en = GetEngine();
		auto& wrld = *world;
		auto& sRen = scene.GetRender();

		auto& modelRender = *scene.GetObjectByName(L"Chunk").GetComponents<ModelRender>()[0];
		//auto mat = &modelRender.GetMaterial(0);
		auto playerChunk = World::TransformToChunkPos(playerPos);
		Vector3i chunkPos = {0,0,0};

		auto newChunkPos = Vector3i();

		std::vector<Vector3i> positions{};
		while (CheckChunkVisible(newChunkPos * 2 + offset)) {
			positions.push_back(newChunkPos * 2 + offset);
			newChunkPos = GetNextPosition(newChunkPos);
		}

		auto posCount = positions.size();

		newChunkPos = { 0,0,0 };
		size_t positionIdx = 0;

		std::function<void(void)> updatePos = [&]() {
			if (positionIdx < posCount)
				newChunkPos = positions[positionIdx++] + Vector3i(playerChunk.x / 2, playerChunk.y / 2, playerChunk.z / 2) * 2;
			else {
				loading[idx] = false;
				Sleep(1);
				positionIdx = 0;
				while (unloading) { Sleep(1); loading[idx] = false; };
				loading[idx] = true;
			}
		};
		updatePos();


		while (enabled) {
			auto newPlayerChunk = World::TransformToChunkPos(playerPos);
			if (newPlayerChunk != playerChunk) {
				playerChunk = newPlayerChunk;
				chunkPos = { 0,0,0 };
			}


			while (unloading) { Sleep(1); loading[idx] = false; };
			loading[idx] = true;

			while (CheckLoaded(newChunkPos, idx)) {
				if (!enabled)
					return;
				chunkPos = GetNextPosition(chunkPos);
				updatePos();
				/*if (!loadingQueue.try_pop(chunkPos)) {
					if (loadingQueue.empty())
						Sleep(20);
					else
						Sleep(1);

					while (unloading) { Sleep(1); };
					loading[idx] = true;
					continue;
				}*/
			}
			if (unloading) continue;
			wrld.SetChunkStatus(newChunkPos, Chunk::Loading);
			auto& chunkObj = *GetNextChunk(idx);
			auto& chunk = world->GetChunk(newChunkPos);
			auto& model = *chunk.GetModel();
			//sRen.WaitRendering(enabled);
			//sRen.Lock();
			if (!enabled)
				return;
			auto render = chunkObj.GetComponents<ModelRender>()[0];
			model.transform.pos = World::TransformToWorldPos(newChunkPos);
			render->SetModel(&model);
			auto n = model.GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				render->SetMaterial(&Voxel::GetMaterialAt(i), i);
			}
			chunkObj.Enable();
			//sRen.Unlock();
			wrld.SetChunkStatus(newChunkPos, Chunk::Loaded);
			loading[idx] = false;
		}
	}

	bool CheckLoaded(Vector3i pos, size_t idx) {
		auto n = chunksPool.size();
		for (auto i = idx; i < n; i += nThreads) {
			auto chunk = chunksPool[i].first;
			if (!chunksPool[i].second)
				continue;
			auto& model = chunk->GetComponents<ModelRender>()[0]->GetModel().transform.pos;
			if (&model == nullptr)
				continue;
			if (World::TransformToChunkPos(model) == pos)
				return true;
		}
		return false;
	}

	SceneObject* GetNextChunk(size_t idx) {
		auto& sRen = GetScene().GetRender();
		auto n = chunksPool.size();
		while (enabled) {
			for (auto i = idx; i < n; i += nThreads) {
				auto chunk = chunksPool[i].first;
				if (!chunksPool[i].second) {
					chunksPool[i].second = true;
					return chunk;
				}
			}
			loading[idx] = false;
			Sleep(1);
			while (unloading) { Sleep(1); loading[idx] = false; }
			loading[idx] = true;
		}
		return nullptr;
	}

	bool CheckLoading() {
		for (auto b : loading) {
			if(b) return true;
		}
		return false;
	}

	void UnloadChunks() {
		auto& sRen = GetScene().GetRender();
		auto playerChunk = World::TransformToChunkPos(playerPos);
		auto queuePos = std::queue<Vector3i>();
		auto queueObj = std::queue< std::pair<SceneObject*,Vector3i>>();

		unloading = true;
		while (CheckLoading()) Sleep(1);
		sRen.Wait(enabled);
		sRen.Lock();

		for (auto it = chunksPool.begin(); it != chunksPool.end(); it++) {
			auto& chunk = *it->first;
			if (chunk.isEnabled()) {
 				auto& model = chunk.GetComponents<ModelRender>()[0]->GetModel();
				if (&model == nullptr) continue;
				auto chunkPos = World::TransformToChunkPos(model.transform.pos);

				if (!CheckChunkVisible(chunkPos - (playerChunk / 2) * 2)) {
					it->second = false;
					queuePos.push(chunkPos); 
					queueObj.push({ &chunk, chunkPos });
				}
			}
		}

		while (!queueObj.empty()) {
			auto o = queueObj.front();
			queueObj.pop();
			o.first->Disable();
			world->SetChunkStatus(o.second, Chunk::Unloaded);
		}
		sRen.Unlock();
		while (!queuePos.empty()) {
			world->UnloadChunk(queuePos.front());
			queuePos.pop();
		}
		unloading = false;//it was risky but still working
				  //if programm was stopped here, move this to the end of Unload

	}

	bool CheckChunkVisible(Vector3i chunkPos, int addedDistance = 0) {
		//chunkPos /= 2;
		//chunkPos *= 2;
		chunkPos.x = Math::Abs(chunkPos.x);
		chunkPos.y = Math::Abs(chunkPos.y);
		chunkPos.z = Math::Abs(chunkPos.z);
		return chunkPos.x + chunkPos.y + chunkPos.z <= renderDistance + addedDistance;
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