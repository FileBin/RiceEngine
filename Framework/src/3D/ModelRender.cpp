#include <GameEngine/ModelRender.h>
namespace GameEngine {
	bool ModelRenderer::Init() {
		auto n = model->GetSubMeshesCount();
		indexBuffers.resize(n);
		vertexBuffers.resize(n);
		for (size_t i = 0; i < n; i++) {
			auto mesh = model->GetSubMesh(i);
			indexBuffers[i] = device->CreateBuffer(mesh.indexBuffer, D3D11_BIND_INDEX_BUFFER);
			vertexBuffers[i] = device->CreateBuffer(mesh.vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		}
		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);
		device->SetPrimitiveTopology();
		return true;
	}

	void ModelRenderer::SetClearColor(Color color) {
		clearColor = color;
	}

	void ModelRenderer::BeginFrame() {
		device->ClearFrame(clearColor);
	}

	bool ModelRenderer::Draw() {
		auto n = model->GetSubMeshesCount();

		ConstantBufferData cb;
		cb.View = XMMatrixIdentity();
		cb.World = XMMatrixTranspose(transform.GetTransformationMatrix());
		cb.Projection = XMMatrixTranspose(camera->GetProjectionMatrix());

		device->LoadBufferSubresource(constantBuffer, cb);
		device->SetActiveVSConstantBuffer(constantBuffer);

		for (size_t i = 0; i < n; i++) {
			device->SetActiveIndexBuffer(indexBuffers[i]);
			device->SetActiveVertexBuffer<Vertex>(vertexBuffers[i]);

			device->Draw();
		}
		return true;
	}

	void ModelRenderer::AddShader(String name, Shader*& sh) {
		shaders.insert(shaders.end(), { name, sh });
	}

	void ModelRenderer::SetModel(Model& model) {
		this->model = &model;
	}

	const Model* ModelRenderer::GetModel() const {
		return model;
	}

	const Shader* ModelRenderer::GetShader(String name) const {
		return shaders.at(name);
	}
}
