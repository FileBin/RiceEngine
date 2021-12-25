#pragma once

#include "RenderBase.h"
#include <vector>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <GameEngine\macros.h>


namespace Game {
	using namespace concurrency;
	class Material;
	class Model;
	class Camera;
	struct Mesh;

	class SceneRender : public RenderBase {
	public:

		bool Init();
		void BeginFrame();
		bool Draw();
		void Close();
		void Resize();

		void Lock() { isLoading = true; }
		void Wait() { while (isLoading || isRendering) Sleep(20); }
		void WaitRendering() { while (isRendering) Sleep(20); }
		void Unlock() { isLoading = false; }

		void AddModel(Model* model);
		bool RemoveModel(Model* model, bool erase = false);
		void UpdateModel(Model* model);
		void MapMaterial(Mesh* mesh, Material* mat);
		void UnmapMaterial(Mesh* mesh);
		void UpdateBuffer(Mesh* mesh);

		void AddCamera(Camera* cam);
		Camera& GetCamera(size_t idx = 0);

		Material& CreateMaterial(Shader* sh);

	private:
		bool isRendering = false, isLoading = false;

		concurrent_vector<Camera*> cameras;
		concurrent_unordered_map<Material*, bool> materials;
		concurrent_unordered_map<Mesh*, Material*> materialMap;
		size_t activeCameraIdx;
		concurrent_unordered_map<Model*, bool> models;
		concurrent_unordered_map<Mesh*, Buffer*> indexBuffers;
		concurrent_unordered_map<Mesh*, Buffer*> vertexBuffers;
		Buffer* constantBuffer;
	};
}