#pragma once

#include <GameEngine\Components\MonoScript.h>
#include <Scripts\World\Chunk.h>
#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>

#include "World\World.h"
#include "World\FlatGenerator.h"

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
	int renderDistance = 4;
	World* world;
	WorldGenerator* generator;

	vector<thread*> threads;
	concurrent_priority_queue<Vector3i, MyComparer> positions{};

	Vector3 playerPos{0, 0, 0};

	void Start() {
		playerPos.y = 60;
		generator = new FlatGenerator(50.);
		world = new World(generator);

		for (num i = -renderDistance; i <= renderDistance; i++) {
			for (num j = -renderDistance; j <= renderDistance; j++) {
				for (num k = -renderDistance; k <= renderDistance; k++) {
					positions.push({ i, j, k });
				}
			}
		}

		threads.resize(1);

		for(auto t : threads){
			t = new thread([this]() { ChunkLoaderThread(); });
		}
	}

	void Update() {
		playerPos = GetRender().GetCamera(0).transform.pos;
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
			auto& chunk = *o.Instaniate();
			auto render = new ModelRender();
			chunk.AddComponent(render);
			render->SetModel(model);
			render->SetMaterial(mat, 0);
			render->Enable();
			wrld.SetChunkStatus(chunkPos, Chunk::Loaded);
		}
	}
};