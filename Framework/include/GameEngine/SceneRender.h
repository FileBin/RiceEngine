#pragma once

#include "RenderBase.h"
#include <vector>
#include <map>

namespace Game {
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
		void MapMaterial(Mesh* mesh, Material* mat);

		void AddCamera(Camera* cam);

		Material& CreateMaterial(Shader* sh);
	private:
		std::vector<Camera*> cameras;
		std::vector<Material*> materials;
		std::map<Mesh*, Material*> materialMap;
		size_t activeCameraIdx;
		std::vector<Model*> models;
		std::map<Mesh*, Buffer*> indexBuffers;
		std::map<Mesh*, Buffer*> vertexBuffers;
		std::vector<Buffer*> constantBuffers;
	};
}