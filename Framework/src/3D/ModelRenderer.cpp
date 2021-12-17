#include <GameEngine/ModelRender.h>
#include <GameEngine\Matrixes.h>
#include "..\..\include\GameEngine\Components\ModelRender.h"
#include <GameEngine\Util3D.h>

namespace Game {

	bool ModelRenderer::Init() {
		auto n = model->GetSubMeshesCount();
		materials.resize(n);
		indexBuffers.resize(n);
		vertexBuffers.resize(n);
		for (size_t i = 0; i < n; i++) {
			auto& mesh = model->GetSubMesh(i);
			vertexBuffers[i] = device->CreateBuffer(mesh.vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			indexBuffers[i] = device->CreateBuffer(mesh.indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
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

		ConstantBufferData cb = {};
		cb.World = transform.GetTransformationMatrix();
		cb.View = camera.GetTransformationMatrix();
		cb.Projection = camera.GetProjectionMatrix();


		device->LoadBufferSubresource(constantBuffer, cb);
		device->SetActiveVSConstantBuffer(constantBuffer);

		for (size_t i = 0; i < n; i++) {
			device->SetActiveVertexBuffer<Vertex>(vertexBuffers[i]);
			device->SetActiveIndexBuffer(indexBuffers[i]);

			auto mat = *(materials[i]);
			device->SetActivePSConstantBuffer(mat.GetBuffer());
			device->SetActiveShader(mat.GetShader());

			device->Draw();
		}
		return true;
	}

	void ModelRenderer::Resize() {
		camera.SetAspectRatio(GetAspectRatio());
	}

	void ModelRenderer::SetMaterial(size_t idx, Material &mat) {
		materials[idx] = &mat;
	}

	void ModelRenderer::SetModel(Model& model) {
		this->model = &model;
	}

	Model& ModelRenderer::GetModel() const {
		return *model;
	}

	Material& ModelRenderer::GetMaterial(size_t idx) const {
		return *(materials[idx]);
	}
}
