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
	concurrent_priority_queue<Vector3i, MyComparer> positions{};
	concurrent_priority_queue<Vector3i, MyComparer> loadingQueue{};

	Vector3 playerPos{0, 0, 0};

	void Start() {
		auto& scene = GetScene();

		playerPos.y = 30;

		//generator = new FlatGenerator(25.);
		generator = new StandartGenerator(WorldSeed::Default(), 60, -20, .3);
		world = new World(generator, &scene.GetRender());

		auto a = renderDistance;

		for (num i = -a; i <= a; i++) {
			for (num j = -a; j <= a; j++) {
				for (num k = -a; k <= a; k++) {
					positions.push({ i, j, k });
				}
			}
		}

		auto n = positions.size();
		chunksPool.resize(n);
		n = chunksPool.size();
		for (size_t i = 0; i < n; i++) {
			auto o = scene.Instaniate();
			auto render = new ModelRender();
			o->AddComponent(render);
			chunksPool[i].first = o;
			chunksPool[i].second = false;
		}
		loadingQueue = positions;

		auto nThreads = 6;
		threads.resize(nThreads);
		loading.resize(nThreads);

		for (size_t i = 0; i < threads.size();i++) {
			threads[i] = new thread(&ChunkGenerator::ChunkLoaderThread, this, i);
		}
	}

	void Update() {
		auto& en = GetEngine();
		auto& ren = GetRender();
		//auto grassMat = ren.GetMaterial(L"DarkGrass");
		//grassMat.SetVar(L"time", (float)en.GetTime());
		//grassMat.UpdateBuffer();

		auto newPos = ren.GetCamera(0).transform.pos;
		auto playerChunk = World::TransformToChunkPos(newPos);
		if (playerChunk != World::TransformToChunkPos(playerPos)) {
			if (!unloading) {
				create_task([this]() { UnloadChunks(); });
				loadingQueue = positions;
			}
			Log::log(Log::INFO, L"ChunkStatus: {}", (int)world->GetChunkStatus(playerChunk));
		}
		playerPos = newPos;
	}

	void ChunkLoaderThread(size_t idx) {

		auto& o = GetSceneObject();
		auto& scene = GetScene();
		auto& en = GetEngine();
		auto& wrld = *world;
		auto& sRen = scene.GetRender();

		auto& modelRender = *scene.GetObjectByName(L"Chunk").GetComponents<ModelRender>()[0];
		//auto mat = &modelRender.GetMaterial(0);

		while (enabled) {
			auto playerChunk = World::TransformToChunkPos(playerPos);
			auto chunkPos = playerChunk;
			while (unloading) { Sleep(1); loading[idx] = false; };
			loading[idx] = true;
			while (wrld.GetChunkStatus(chunkPos) > 0) {
				if (!enabled)
					return;
				loading[idx] = false;
				if (!loadingQueue.try_pop(chunkPos)) {
					if (loadingQueue.empty())
						Sleep(20);
					else
						Sleep(1);

					while (unloading) { Sleep(1); };
					loading[idx] = true;
					continue;
				}
				chunkPos = chunkPos + playerChunk;
				while (unloading) { Sleep(1); loading[idx] = false; };
				loading[idx] = true;
			}
			wrld.SetChunkStatus(chunkPos, Chunk::Loading);
			auto& chunkObj = GetNextChunk(idx);
			auto& chunk = world->GetChunk(chunkPos);
			auto& model = *chunk.GetModel();
			sRen.WaitRendering(enabled);
			sRen.Lock();
			if (!enabled)
				return;
			auto render = chunkObj.GetComponents<ModelRender>()[0];
			model.transform.pos = World::TransformToWorldPos(chunkPos);
			render->SetModel(&model);
			auto n = model.GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				render->SetMaterial(&Voxel::GetMaterialAt(i), i);
			}
			chunkObj.Enable();
			sRen.Unlock();
			wrld.SetChunkStatus(chunkPos, Chunk::Loaded);
			loading[idx] = false;
		}
	}

	SceneObject& GetNextChunk(size_t idx) {
		auto& sRen = GetScene().GetRender();
		while (enabled) {
			for (auto it = chunksPool.begin(); it != chunksPool.end(); it++) {
				auto& chunk = *it->first;
				if (!it->second) {
					it->second = true;
					return chunk;
				}
			}
			loading[idx] = false;
			//more threads -> moreTimeout
			Sleep(50);
			while (unloading) { Sleep(1); loading[idx] = false; }
			loading[idx] = true;
		}
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

		for (auto it = chunksPool.begin(); it != chunksPool.end(); it++) {
			auto& chunk = *it->first;
			if (chunk.isEnabled()) {
 				auto& model = chunk.GetComponents<ModelRender>()[0]->GetModel();
				if (&model == nullptr) continue;
				auto chunkPos = World::TransformToChunkPos(model.transform.pos);

				Vector3i d = {
					abs(chunkPos.x - playerChunk.x),
					abs(chunkPos.y - playerChunk.y),
					abs(chunkPos.z - playerChunk.z),
				};
				if (d.x > renderDistance || d.y > renderDistance || d.z > renderDistance) {
					it->second = false;
					queuePos.push(chunkPos); 
					queueObj.push({ &chunk, chunkPos });
				}
			}
		}

		sRen.Wait(enabled);
		sRen.Lock();
		while (!queueObj.empty()) {
			auto o = queueObj.front();
			queueObj.pop();
			o.first->Disable();
			world->SetChunkStatus(o.second, Chunk::Unloaded);
		}
		sRen.Unlock();
		unloading = false;//it was risky but still working
						  //if programm was stopped here, move this to the end of Unload
		while (!queuePos.empty()) {
			world->UnloadChunk(queuePos.front());
			queuePos.pop();
		}
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