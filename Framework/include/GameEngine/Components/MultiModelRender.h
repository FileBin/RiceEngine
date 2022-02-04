#pragma once

#include "../Scene/Component.h"
#include <GameEngine/macros.h>
#include "../Model.h"
#include "../DX/RenderingMesh.h"
#include "../Util/RegisterCollection.h"

namespace Game {

	using std::vector;

	class Material;

	class MultiModelRender : public Component, public IRenderable {
	public:
		struct UUID {
		private:
			friend class MultiModelRender;
			size_t keys_count;
			size_t* keys;
			SmartPtr<MultiModelRender> ptr;
		public:
			bool IsActive();
			SmartPtr<MultiModelRender> GetRender() { return ptr; }
		};

		void OnInit();

		void OnEnable();
		void OnDisable();

		/// <summary>
		/// Sets the materials used to render given submeshes
		/// </summary>
		/// <param name="materials"> - array of materials (any of materials can be null)</param>
		void SetMaterials(std::vector<SmartPtr<Material>> materials);

		/// <summary>
		/// Return the material reference used to rendering submeshes at given index
		/// </summary>
		/// <param name="subMeshIdx"> - index of given submeshes</param>
		/// <returns></returns>
		SmartPtr<Material> GetMaterial(size_t subMeshIdx) const { return meshes[subMeshIdx].material; }

		/// <summary>
		/// Removes Model from rendering pipeline by UUID (thread safe)
		/// </summary>
		/// <param name="idx"> UUID of removable Model </param>
		void RemoveModel(MultiModelRender::UUID idx);

		/// <summary>
		/// Add Model to rendering pipeline (thread safe)
		/// </summary>
		/// <param name="model"> - model reference (not null)</param>
		/// <param name="transformationMatrix"> - 4x4 transformation matrix applied to the model</param>
		/// <returns> UUID to manage the model in collection </returns>
		UUID AddModel(SmartPtr<Model> model, Matrix4x4 transformationMatrix = Matrix4x4::identity);

		/// <summary>
		/// Changes the model on given index (thread safe)
		/// </summary>
		/// <param name="idx"></param>
		/// <param name="model"></param>
		/// <param name="transformationMatrix"></param>
		void SetModel(UUID uuid, SmartPtr<Model> model, Matrix4x4 transformationMatrix = Matrix4x4::identity);

		void Render(Matrix4x4&, Matrix4x4&, Matrix4x4&, RenderType type);
		void Render(Matrix4x4, Matrix4x4, Matrix4x4);
		void RenderTransparent(Matrix4x4, Matrix4x4, Matrix4x4);

		void Register(size_t);
		size_t Unregister();
	private:
		struct RenderData {
			Microsoft::WRL::ComPtr<Buffer> indexBuffer{ 0 }, vertexBuffer{ 0 };

			SmartPtr<Mesh> mesh;
		};

		struct MeshData {
			SmartPtr<Material> material;
			RegisterCollection<SmartPtr<RenderData>> meshes;
		};

		SmartPtr<Transform> transform;
		size_t idx;
		_STD mutex renderMutex;
		SmartPtr<Device> device;

		_STD vector<MeshData> meshes;

		void setActiveMaterial(SmartPtr<Material> mat);

		void setupRenderData(RenderData* data, SmartPtr<Mesh> pMesh, Matrix4x4& transformationMatrix);
	};
}
