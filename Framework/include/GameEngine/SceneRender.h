﻿#pragma once

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

		void Lock(size_t idx) { isLoading[idx] = true; }

		bool CheckLoading() {
			for (byte i = 0; i < 0xff; i++)
				if (isLoading[i]) return true;
			return false;
		}

		void Wait(bool& livingFactor) { while (livingFactor && (CheckLoading() || isRendering)) Sleep(1); }
		void WaitRendering(bool& livingFactor) { while (livingFactor && isRendering) Sleep(1); }

		void Wait() { while (isLoading || isRendering) Sleep(1); }
		void WaitRendering() { while (isRendering) Sleep(1); }

		void Unlock(size_t idx) { isLoading[idx] = false; }

		void AddModel(std::shared_ptr<Model> model);
		bool RemoveModel(std::shared_ptr<Model> model, bool erase = false);
		void UpdateModel(std::shared_ptr<Model> model);
		void MapMaterial(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat);
		void UnmapMaterial(std::shared_ptr<Mesh> mesh);
		void UpdateBuffer(std::shared_ptr<Mesh> mesh);

		void AddCamera(Camera* cam);
		Camera& GetCamera(size_t idx = 0);

		Shader* CreateShader(String name);
		Shader& GetShader(String name);

		std::shared_ptr<Material> CreateMaterial(String name, Shader* sh, std::vector<std::pair<String, size_t>> mapping = {});
		std::shared_ptr<Material> GetMaterial(String name);

		Texture2D& CreateTexture(String filename) { return *device->CreateTexture(filename); }
		Texture2D& GetDepthBufferTex() { return *device->GetDepthBufferTex(); }

		void AddDrawable(UI::IDrawable* txt);
		void RemoveDrawable(UI::IDrawable* txt);

	private:
		bool isRendering = false;
		bool isLoading[0xff];
		concurrent_vector<Camera*> cameras;
		concurrent_unordered_map<String, std::shared_ptr<Material>> materials;
		concurrent_unordered_map<String, Shader*> shaders;
		concurrent_unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<Material>> materialMap;
		size_t activeCameraIdx;
		concurrent_unordered_map<std::shared_ptr<Model>, bool> models; //that was a trick beacuse unordered_set doesn't work
		concurrent_unordered_map<UI::IDrawable*, bool> texts;
		concurrent_unordered_map<std::shared_ptr<Mesh>, ConstantBufferData> transparentQ;
		concurrent_unordered_map<std::shared_ptr<Mesh>, Buffer*> indexBuffers;
		concurrent_unordered_map<std::shared_ptr<Mesh>, Buffer*> vertexBuffers;
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