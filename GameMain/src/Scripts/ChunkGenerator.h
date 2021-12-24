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


using namespace Game;
using std::thread, std::vector, concurrency::concurrent_priority_queue;

class MyComparer {
public:
	bool operator()(Vector3i a, Vector3i b) {
		return (a.x * a.x + a.y * a.y + a.z * a.z) > (b.x * b.x + b.y * b.y + b.z * b.z);
	}
};

class ChunkGenerator : public MonoScript {
	//Chunk* chunk;
	int renderDistance = 12;
	World* world;
	WorldGenerator* generator;

	vector<thread*> threads;
	concurrent_vector<SceneObject*> chunksPool{};
	concurrent_priority_queue<Vector3i, MyComparer> positions{};

	Vector3 playerPos{0, 0, 0};

	void Start() {
		auto& scene = GetScene();

		playerPos.y = 60;

		//generator = new FlatGenerator(50.);
		generator = new StandartGenerator(WorldSeed::Default(),60, -20, .3);
		world = new World(generator);

		auto a = renderDistance;

		for (num i = -a; i <= a; i++) {
			for (num j = -a; j <= a; j++) {
				for (num k = -a; k <= a; k++) {
					positions.push({ i, j, k });
				}
			}
		}

		//fix multi threading
		threads.resize(1);

		for(auto t : threads){
			t = new thread([this]() { ChunkLoaderThread(); });
		}

		chunksPool.resize(positions.size());

		for (auto it = chunksPool.begin(); it != chunksPool.end(); it++) {
			auto& o = *it;
			o = scene.Instaniate();
			auto render = new ModelRender();
			o->AddComponent(render);
		}
	}

	void Update() {
		playerPos = GetRender().GetCamera(0).transform.pos;
		UnloadChunks();
	}

	void ChunkLoaderThread(){
		auto& o = GetSceneObject();
		auto& scene = GetScene();
		auto& en = GetEngine();
		auto& wrld = *world;

		auto& modelRender = *scene.GetObjectByName(L"Chunk").GetComponents<ModelRender>()[0];
		auto mat = &modelRender.GetMaterial(0);

		auto positions = this->positions;

		auto playerChunk = World::TransformToChunkPos(playerPos);

		std::function<void(void)> checkPlayerChunk = [&]() {
			auto newPlayerChunk = World::TransformToChunkPos(playerPos);
			if (newPlayerChunk != playerChunk) {
				positions = this->positions;
				playerChunk = newPlayerChunk;
			}
		};

		while(true) {
			checkPlayerChunk();
			auto chunkPos = playerChunk;
			while (wrld.GetChunkStatus(chunkPos) > 0) {
				if (!positions.try_pop(chunkPos)) {
					Sleep(100);
					checkPlayerChunk();
					continue;
				}
				chunkPos = chunkPos + playerChunk;
			}
			wrld.SetChunkStatus(chunkPos, Chunk::Loading);
			auto model = wrld.GetChunk(chunkPos).GetModel();
			model->transform.pos = World::TransformToWorldPos(chunkPos);

			auto& chunk = GetNextChunk();
			auto render = chunk.GetComponents<ModelRender>()[0];
			render->SetModel(model);
			render->SetMaterial(mat, 0);
			chunk.Enable();

			wrld.SetChunkStatus(chunkPos, Chunk::Loaded);
		}
	}

	SceneObject& GetNextChunk() {
		while (true) {
			for (auto it = chunksPool.begin(); it != chunksPool.end(); it++) {
				auto& chunk = **it;
				if (!chunk.isEnabled()) {
					return chunk;
				}
			}
			Sleep(20);
		}
	}

	void UnloadChunks() {
		auto playerChunk = World::TransformToChunkPos(playerPos);
		for (auto it = chunksPool.begin(); it != chunksPool.end(); it++) {
			auto& chunk = **it;
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
					chunk.Disable();
					world->UnloadChunk(chunkPos);
				}
			}
		}
	}

	//create destructors
};