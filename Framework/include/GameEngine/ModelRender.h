#pragma once
#include "stdafx.h"
#include "RenderBase.h"
#include "Util.h"
#include "Model.h"
#include "Camera.h"

namespace Game {
	class ModelRenderer : public RenderBase {
	public:
		ModelRenderer() : RenderBase(), camera() {}
		~ModelRenderer() {}

		bool Init();
		void BeginFrame();
		bool Draw();
		void Close() {}
		void Resize();

		void SetClearColor(Color color);
		void SetMaterial(size_t subMeshIdx, Material& material);
		void SetModel(Model& model);
		Model& GetModel() const;
		Material& GetMaterial(size_t subMeshIdx) const;

		Transform transform;
		Camera camera;
	private:
		Color clearColor = { 0.1f, 0.15f, 0.6f, 1.f };
		std::vector<Material*> materials;
		Model* model = nullptr;
		std::vector<Buffer*> indexBuffers, vertexBuffers;
		Buffer* constantBuffer = nullptr;
	};
}