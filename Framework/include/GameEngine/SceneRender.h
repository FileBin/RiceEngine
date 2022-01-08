﻿#pragma once

#include "RenderBase.h"
#include <vector>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <GameEngine\macros.h>
#include <GameEngine\Vectors\Hasher.h>


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

		std::mutex& Lock(size_t idx) { return isLoading[idx]; }

		void AddModel(SmartPtr<Model> model);
		bool RemoveModel(SmartPtr<Model> model, bool erase = false);
		void UpdateModel(SmartPtr<Model> model);
		void MapMaterial(SmartPtr<Mesh> mesh, SmartPtr<Material> mat);
		void UnmapMaterial(SmartPtr<Mesh> mesh);
		void UpdateBuffer(SmartPtr<Mesh> mesh);

		void AddCamera(SmartPtr<Camera> cam);
		SmartPtr<Camera> GetCamera(size_t idx);
		SmartPtr<Camera> GetActiveCamera() { return GetCamera(activeCameraIdx); }

		Shader* CreateShader(String name);
		Shader& GetShader(String name);

		SmartPtr<Material> CreateMaterial(String name, Shader* sh, std::vector<std::pair<String, size_t>> mapping = {});
		SmartPtr<Material> GetMaterial(String name);

		Texture2D& CreateTexture(String filename) { return *device->CreateTexture(filename); }
		Texture2D& GetDepthBufferTex() { return *device->GetDepthBufferTex(); }

		void AddDrawable(UI::IDrawable* txt);
		void RemoveDrawable(UI::IDrawable* txt);

	private:
		std::mutex isRendering;
		std::mutex isLoading[0x100];
		concurrent_vector<SmartPtr<Camera>> cameras;
		concurrent_unordered_map<String, SmartPtr<Material>> materials;
		concurrent_unordered_map<String, Shader*> shaders;
		concurrent_unordered_map<Mesh*, SmartPtr<Material>> materialMap;
		size_t activeCameraIdx;
		concurrent_unordered_map<SmartPtr<Model>, bool> models; //that was a trick beacuse unordered_set doesn't work
		concurrent_unordered_map<UI::IDrawable*, bool> texts;
		concurrent_unordered_map<SmartPtr<Mesh>, Buffer*> indexBuffers;
		concurrent_unordered_map<SmartPtr<Mesh>, Buffer*> vertexBuffers;
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