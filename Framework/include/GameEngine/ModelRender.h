#pragma once
#include "stdafx.h"
#include "RenderBase.h"
#include "Util.h"
#include "Model.h"
#include "Camera.h"

namespace Game {
	class ModelRenderer : public RenderBase {
	public:
		ModelRenderer() : RenderBase() {}
		~ModelRenderer() {}

		bool Init();
		void BeginFrame();
		bool Draw();
		void Close();
		void Resize();

		void SetClearColor(Color color);
		void AddShader(String name, Shader*& sh);
		void SetModel(Model& model);
		const Model* GetModel() const;
		const Shader* GetShader(String name) const;

		Transform transform;
		Camera* camera = nullptr;
	private:
		Color clearColor = { 0.1f, 0.15f, 0.6f, 1.f };
		Map<String, Shader*> shaders = {};
		Model* model = nullptr;
		List<Buffer*> indexBuffers = {}, vertexBuffers = {};
		Buffer* constantBuffer = nullptr;
	};
}