#pragma once

#include "../Scene/Component.h"
#include <GameEngine/macros.h>
#include "../Model.h"
#include <concurrent_vector.h>
#include "../DX/RenderingMesh.h"

namespace Game {

	using std::vector;

	class Material;

	class ModelRender : public Component, public IRenderable {
	public:
		void OnInit();

		void OnEnable();
		void OnDisable();

		void Register(size_t);
		size_t Unregister();

		void DeleteModel() { model.Release(); }

		void SetMaterial(SmartPtr<Material> material, size_t subMeshIdx);
		void SetModel(SmartPtr<Model> model);
		SmartPtr<Model> GetModel();
		SmartPtr<Material> GetMaterial(size_t subMeshIdx) const;

		void Render(Matrix4x4&, Matrix4x4&, Matrix4x4&, RenderType type);
		void Render(Matrix4x4, Matrix4x4, Matrix4x4);
		void RenderTransparent(Matrix4x4, Matrix4x4, Matrix4x4);
	private:
		struct RenderData {
			Microsoft::WRL::ComPtr<Buffer> indexBuffer{ 0 }, vertexBuffer{ 0 };
			SmartPtr<Material> material;
		};

		SmartPtr<Transform> transform;
		size_t idx;
		std::mutex renderMutex;
		::std::shared_mutex modelMutex;
		SmartPtr<Device> device;
		SmartPtr<Model> model;
		std::vector<RenderData> meshes;

		void setActiveMaterial(SmartPtr<Material> mat);
	};
}
