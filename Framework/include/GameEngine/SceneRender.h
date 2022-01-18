#pragma once

#include "RenderBase.h"
#include <vector>
#include <concurrent_unordered_set.h>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <GameEngine\macros.h>
#include <GameEngine\Vectors\Hasher.h>
#include "Util\SmartPointer.h"

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
			SmartPtr<Mesh> orig = nullptr;
			SmartPtr<Vector3> pPos = nullptr;
			SmartPtr<Vector3> pScale = nullptr;
			SmartPtr<Quaternion> pRot = nullptr;
			Microsoft::WRL::ComPtr<Buffer> pIndexBuffer = nullptr;
			Microsoft::WRL::ComPtr<Buffer> pVertexBuffer = nullptr;
			SmartPtr<Material> pMat = nullptr;
			void Draw(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection, LightManager* mgr = nullptr, bool checkVisiblity = true);

			void DrawShadow(SceneRender* ren, Matrix4x4f View, Matrix4x4f Projection);

			~RenderingMesh() {
				pIndexBuffer->Release();
				pVertexBuffer->Release();
			}
		};

		class LightManager {
			struct LightBuffer {
				Matrix4x4f LVP = Matrix4x4f::identity;
				Vector4f ambient;
				Vector4f diffuse;
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
		public:
			void Init(SceneRender* ren, std::vector<dbl> mapSizes, dbl shadowDistanse = 300, size_t shadowMapRes = 1024);
			void RenderShadowMap(Vector3 playerPos);

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