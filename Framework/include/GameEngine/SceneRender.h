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
	namespace UI {
		__interface IDrawable;
	}

	class SceneRender : public RenderBase {
	public:

		bool Init();
		void BeginFrame();
		bool Draw();
		void Close();
		void Resize();

		void Lock() { isLoading = true; }

		void Wait(bool& livingFactor) { while (livingFactor && (isLoading || isRendering)) Sleep(1); }
		void WaitRendering(bool& livingFactor) { while (livingFactor && isRendering) Sleep(1); }

		void Wait() { while (isLoading || isRendering) Sleep(1); }
		void WaitRendering() { while (isRendering) Sleep(1); }

		void Unlock() { isLoading = false; }

		void AddModel(Model* model);
		bool RemoveModel(Model* model, bool erase = false);
		void UpdateModel(Model* model);
		void MapMaterial(Mesh* mesh, Material* mat);
		void UnmapMaterial(Mesh* mesh);
		void UpdateBuffer(Mesh* mesh);

		void AddCamera(Camera* cam);
		Camera& GetCamera(size_t idx = 0);

		Shader* CreateShader(String name);
		Shader& GetShader(String name);

		Material& CreateMaterial(String name, Shader* sh, std::vector<std::pair<String, size_t>> mapping = {});
		Material& GetMaterial(String name);

		Texture2D& CreateTexture(String filename) { return *device->CreateTexture(filename); }
		Texture2D& GetDepthBufferTex() { return *device->GetDepthBufferTex(); }

		void AddDrawable(UI::IDrawable* txt);
		void RemoveDrawable(UI::IDrawable* txt);

	private:
		bool isRendering = false, isLoading = false;
		concurrent_vector<Camera*> cameras;
		concurrent_unordered_map<String, Material*> materials;
		concurrent_unordered_map<String, Shader*> shaders;
		concurrent_unordered_map<Mesh*, Material*> materialMap;
		size_t activeCameraIdx;
		concurrent_unordered_map<Model*, bool> models; //that was a trick beacuse unordered_set doesn't work
		concurrent_unordered_map<UI::IDrawable*, bool> texts;
		concurrent_unordered_map<Mesh*, ConstantBufferData> transparentQ;
		concurrent_unordered_map<Mesh*, Buffer*> indexBuffers;
		concurrent_unordered_map<Mesh*, Buffer*> vertexBuffers;
		Buffer* constantBuffer;

		struct RenderingMesh {
			Mesh* orig = nullptr;
			Vector3* pPos = nullptr;
			Vector3* pScale = nullptr;
			Quaternion* pRot = nullptr;
			Buffer* pIndexBuffer = nullptr;
			Buffer* pVertexBuffer = nullptr;
			Material* pMat = nullptr;
		};
	};
}