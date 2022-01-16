#pragma once

#include "RenderBase.h"
#include <vector>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <GameEngine\macros.h>
#include <GameEngine\Vectors\Hasher.h>
#include "Util\SmartPointer.h"


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
		class RenderingMesh {
		public:
			SmartPtr<Mesh> orig = nullptr;
			SmartPtr<Vector3> pPos = nullptr;
			SmartPtr<Vector3> pScale = nullptr;
			SmartPtr<Quaternion> pRot = nullptr;
			Microsoft::WRL::ComPtr<Buffer> pIndexBuffer = nullptr;
			Microsoft::WRL::ComPtr<Buffer> pVertexBuffer = nullptr;
			SmartPtr<Material> pMat = nullptr;
			void Draw(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection, bool ckeckVisiblity = true);
			~RenderingMesh() {
				pIndexBuffer->Release();
				pVertexBuffer->Release();
			}
		};
	public:

		bool Init();
		void BeginFrame();
		bool Draw();
		void Close();
		void Resize();

		void SetupSkybox(SmartPtr<Material> skyboxMat);

		void AddModel(SmartPtr<Model> model, std::vector<SmartPtr<Material>> materials);
		void RemoveModel(SmartPtr<Model> model);
		void ChangeModel(SmartPtr<Model> removemodel, SmartPtr<Model> replace, std::vector<SmartPtr<Material>> materials);
		bool UpdateBuffers(SmartPtr<Mesh> mesh);

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

	private:
		size_t activeCameraIdx;
		std::mutex m_mutex, m_2dMutex;
		concurrency::concurrent_vector<SmartPtr<Camera>> cameras;
		concurrency::concurrent_unordered_map<String, SmartPtr<Material>> materials;
		concurrency::concurrent_unordered_map<String, SmartPtr<Shader>> shaders;
		std::unordered_map<SmartPtr<Mesh>, SmartPtr<RenderingMesh>> renderingMeshes, transparentMeshes;
		std::vector<SmartPtr<UI::IDrawable>> drawables;
		Microsoft::WRL::ComPtr<Buffer> constantBuffer;

		//default
		SmartPtr<RenderingMesh> skyBox, postProcessingQuad;
		SmartPtr<Material> skyboxMaterial;

		Mesh* CreateSkyBoxMesh();
	};
}