#pragma once

#include "RenderBase.h"
#include <vector>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>

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
		void Close() {}
		void Resize();

		void AddModel(Model* model);
		void RemoveModel(Model* model, bool erase = false);
		void UpdateModel(Model* model);
		void MapMaterial(Mesh* mesh, Material* mat);
		void UnmapMaterial(Mesh* mesh);
		void UpdateBuffer(Mesh* mesh);

		void AddCamera(Camera* cam);
		Camera& GetCamera(size_t idx = 0);

		Material& CreateMaterial(Shader* sh);

	private:
		bool isRendering = false;

		concurrent_vector<Camera*> cameras;
		concurrent_vector<Material*> materials;
		concurrent_unordered_map<Mesh*, Material*> materialMap;
		size_t activeCameraIdx;
		concurrent_vector<Model*> models;
		concurrent_unordered_map<Mesh*, Buffer*> indexBuffers;
		concurrent_unordered_map<Mesh*, Buffer*> vertexBuffers;
		Buffer* constantBuffer;
	};
}