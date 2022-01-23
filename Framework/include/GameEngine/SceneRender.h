﻿#pragma once

#include "RenderBase.h"
#include <vector>
#include <queue>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <GameEngine\macros.h>
#include <GameEngine\Vectors\Hasher.h>
#include "Util\SmartPointer.h"
#include "Scene\IPostProcess.h"
#include <unordered_set>
#include "Components\ModelRender.h"
#include "Components\Transform.h"

struct Matrix4x4f;

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
	private:
#pragma region Classes
		class LightManager;

		class RenderingMesh {
		public:
			SmartPtr<Mesh> orig;
			SmartPtr<Transform> transform;
			Microsoft::WRL::ComPtr<Buffer> pIndexBuffer = nullptr;
			Microsoft::WRL::ComPtr<Buffer> pVertexBuffer = nullptr;
			void Draw(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection, Matrix4x4 LVP = Matrix4x4::identity, bool checkVisiblity = true);

			void DrawShadow(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection);

			~RenderingMesh() {
				pIndexBuffer->Release();
				pVertexBuffer->Release();
			}
		};

		typedef std::unordered_map<SmartPtr<Mesh>, SmartPtr<RenderingMesh>> RenderingMeshCollection;

		class LightManager {
			struct LightBuffer {
				Vector4f ambient;
				Vector4f diffuse;
				Vector4f lightDir;
			} buf;
			SmartPtr<Texture2D> shadowAtlas;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTex;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
			Microsoft::WRL::ComPtr<Buffer> lightBuffer, constantBuffer;
			SceneRender* sceneRender;

			size_t numCascades;
			Vector3 lightDirection;
			dbl shadowDistance = 300;
			size_t shadowMapRes;
			std::vector<dbl> shadowMapSizes;
			Matrix4x4 LVP = Matrix4x4::identity;
		public:
			void UpdateBuffer();
			void PreInit(SceneRender* ren, std::vector<dbl> mapSizes, dbl shadowDistanse = 600, size_t shadowMapRes = 1024);
			void RenderShadowMap(Vector3 playerPos, std::unordered_map<SmartPtr<Material>, RenderingMeshCollection>& meshes);

			Matrix4x4 GetMatrixLVP() { return LVP; }

			Texture2D* GetShadowMap() { return shadowAtlas.Get(); }
			Buffer* GetBuffer() { return lightBuffer.Get(); }
			
		} lightManager;
#pragma endregion

		friend class LightManager;
	public:

		bool Init();
		void BeginFrame();
		bool Draw();
		void Close();
		void Resize();

		void SetupSkybox(SmartPtr<Material> skyboxMat);

		void AddModel(ModelRender* mr, Transform* transform);
		void RemoveModel(SmartPtr<Model> mod);
		void ChangeModel(ModelRender* ren, Transform* tr, SmartPtr<Model> mod);
		bool UpdateBuffers(Mesh* mesh);

		void AddCamera(SmartPtr<Camera> cam);
		SmartPtr<Camera> GetCamera(size_t idx);
		SmartPtr<Camera> GetActiveCamera() { return GetCamera(activeCameraIdx); }

		SmartPtr<Shader> CreateShader(String name);
		SmartPtr<Shader> GetShader(String name);

		SmartPtr<Material> CreateMaterial(String name, SmartPtr<Shader> sh, std::vector<std::pair<String, size_t>> mapping = {});
		SmartPtr<Material> GetMaterial(String name);

		Texture2D& CreateTexture(String filename) { return *device->CreateTexture(filename); }
		Texture2D& GetDepthBufferTex() { return *device->GetDepthBufferTex(); }
		Texture2D& GetRenderTargetTex() { return *device->GetRenderTargetTex(); }

		void AddDrawable(UI::IDrawable* txt);
		void RemoveDrawable(UI::IDrawable* txt);

		void PostProcess(Material* mat);

		void AddPostProcessScript(IPostProcess* ppscript) { ppscripts.insert(ppscript); }
		void RemovePostProcessScript(IPostProcess* ppscript) { ppscripts.erase(ppscript); }

	private:


		size_t activeCameraIdx;
		std::mutex m_mutex, m_2dMutex, m_removeMutex;
		concurrency::concurrent_vector<SmartPtr<Camera>> cameras;
		std::unordered_map<String, SmartPtr<Material>> materials;
		std::unordered_map<String, SmartPtr<Shader>> shaders;
		std::unordered_map<SmartPtr<Material>, RenderingMeshCollection> renderingMeshes, transparentMeshes;
		std::unordered_set<SmartPtr<IPostProcess>> ppscripts;
		std::vector<SmartPtr<UI::IDrawable>> drawables;
		Microsoft::WRL::ComPtr<Buffer> constantBuffer;

		std::queue<SmartPtr<Model>> removeQ;
		std::queue<std::pair<ModelRender*, Transform*>> addQ;

		//default
		SmartPtr<RenderingMesh> skyBox, postProcessingQuad;
		SmartPtr<Material> skyboxMaterial;

		Mesh* CreateSkyBoxMesh();

		void updateMeshes();
		void setActiveMaterial(SmartPtr<Material> mat);
	};
}