#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>

namespace Game {
	bool SceneRender::Init() {
		return true;
	}
	void SceneRender::BeginFrame() {
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
	}
	bool SceneRender::Draw() {
		auto n = models.size();
		for (size_t i = 0; i < n; i++) {
			auto& model = models[i];
			auto m = model->GetSubMeshesCount();
			for (size_t j = 0; j < m; j++) {
				auto& mesh = model->GetSubMesh(j);
				auto ib = indexBuffers.at(&mesh);
				auto vb = vertexBuffers.at(&mesh);

				device->SetActiveVertexBuffer<Vertex>(vb);
				device->SetActiveIndexBuffer(ib);

				auto mat = materialMap.at(&mesh);
				device->SetActivePSConstantBuffer(mat->GetBuffer());
				device->SetActiveShader(mat->GetShader());

				device->Draw();
			}
		}
		return true;
	}

	void SceneRender::Resize() {
		auto n = cameras.size();
		auto ar = GetAspectRatio();
		for (size_t i = 0; i < n; i++) {
			cameras[i]->SetAspectRatio(ar);
		}
	}

	void SceneRender::AddModel(Model* model) {
		models.push_back(model);

		auto n = model->GetSubMeshesCount();

		for (size_t i = 0; i < n; i++) {
			auto& mesh = model->GetSubMesh(i);
			auto vertexBuffer = device->CreateBuffer(mesh.vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto indexBuffer = device->CreateBuffer(mesh.indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE);

			vertexBuffers.insert(vertexBuffers.end(), { &mesh, vertexBuffer });
			indexBuffers.insert(indexBuffers.end(), { &mesh, indexBuffer });
		}
		constantBuffers.push_back(device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER));
	}

	void SceneRender::MapMaterial(Mesh* mesh, Material* mat) { materialMap.insert(materialMap.end(), {mesh, mat}); }

	Material& SceneRender::CreateMaterial(Shader* sh) {
		auto mat = new Material({}, *device, *sh);
		materials.push_back(mat);
		return *mat;
	}
}