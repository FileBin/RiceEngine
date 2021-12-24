#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>

namespace Game {
	bool SceneRender::Init() {
		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);
		return true;
	}
	void SceneRender::BeginFrame() {
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
	}
	bool SceneRender::Draw() {
		isRendering = true;
		device->SetPrimitiveTopology();
		auto n = models.size();
		for (size_t i = 0; i < n; i++) {
			auto& model = models[i];

			ConstantBufferData cb = {};

			auto cam = cameras[activeCameraIdx];
			cb.World = model->transform.GetTransformationMatrix();
			cb.View = cam->GetTransformationMatrix();
			cb.Projection = cam->GetProjectionMatrix();


			device->LoadBufferSubresource(constantBuffer, cb);
			device->SetActiveVSConstantBuffer(constantBuffer);

			auto m = model->GetSubMeshesCount();
			for (size_t j = 0; j < m; j++) {
				auto& mesh = model->GetSubMesh(j);
				Buffer *ib, *vb;
				auto iIt = indexBuffers.find(&mesh);
				if (iIt != indexBuffers.end()) {
					ib = iIt->second;
					vb = vertexBuffers.at(&mesh);
				} else {
					continue;
				}

				device->SetActiveVertexBuffer<Vertex>(vb);
				device->SetActiveIndexBuffer(ib);

				auto matIt = materialMap.find(&mesh);
				if (matIt == materialMap.end()) continue;
				auto& mat = matIt->second;
				device->SetActivePSConstantBuffer(mat->GetBuffer());
				device->SetActiveShader(mat->GetShader());

				device->Draw();
			}
		}
		isRendering = false;
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
		auto n = model->GetSubMeshesCount();

		bool b = false;

		for (size_t i = 0; i < n; i++) {
			auto& mesh = model->GetSubMesh(i);
			if (mesh.indexBuffer.size() == 0) continue;
			b = true;
			auto vertexBuffer = device->CreateBuffer(mesh.vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto indexBuffer = device->CreateBuffer(mesh.indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE);

			vertexBuffers.insert(vertexBuffers.end(), { &mesh, vertexBuffer });
			indexBuffers.insert(indexBuffers.end(), { &mesh, indexBuffer });
		}
		if (b) {
			models.push_back(model);
		}
	}

	void SceneRender::RemoveModel(Model* _model, bool del) {
		while (isRendering)
			Sleep(1);

		bool founded = false;

		for (auto it = models.begin(); it != models.end(); it++) {
			auto& model = *it;
			if (!founded) {
				if (model == _model) {
					founded = true;
					if (model != nullptr) {
						auto n = model->GetSubMeshesCount();
						for (size_t i = 0; i < n; i++) {
							auto& mesh = model->GetSubMesh(i);
							UnmapMaterial(&mesh);

							auto vIt = vertexBuffers.find(&mesh);
							if (vIt != vertexBuffers.end()) {
								vIt->second->Release();
								vertexBuffers.unsafe_erase(&mesh);
							}
							auto it = indexBuffers.find(&mesh);
							if (it != indexBuffers.end()) {
								it->second->Release();
								indexBuffers.unsafe_erase(&mesh);
							}
						}

						

						if (del) {
							delete model;
						}
						auto next = it + 1;
						if (next != models.end())
							model = *next;
					}
				}
			} else {
				auto next = it + 1;
				if (next != models.end())
					model = *next;
			}
		}
		if (founded)
			models.resize(models.size() - 1);
	}

	void SceneRender::UpdateModel(Model* model) {

		auto n = model->GetSubMeshesCount();

		for (size_t i = 0; i < n; i++) {
			auto& mesh = model->GetSubMesh(i);

			auto vertexBuffer = device->CreateBuffer(mesh.vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto indexBuffer = device->CreateBuffer(mesh.indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto it = vertexBuffers.find(&mesh);
			if (it != vertexBuffers.end()) {
				it->second = vertexBuffer;
			} else {
				vertexBuffers.insert(vertexBuffers.end(), { &mesh, vertexBuffer });
			}
			it = indexBuffers.find(&mesh);
			if (it != indexBuffers.end()) {
				it->second = indexBuffer;
			} else {
				indexBuffers.insert(indexBuffers.end(), { &mesh, indexBuffer });
			}
		}
	}



	void SceneRender::MapMaterial(Mesh* mesh, Material* mat) { materialMap.insert(materialMap.end(), {mesh, mat}); }

	void SceneRender::UnmapMaterial(Mesh* mesh) {
		while (isRendering)
			Sleep(1);

		auto it = materialMap.find(mesh);
		if (it != materialMap.end()) {
			materialMap.unsafe_erase(it);
		}
	}

	void SceneRender::UpdateBuffer(Mesh* mesh) {
		auto ib = indexBuffers.at(mesh);
		device->UpdateBufferData(ib, mesh->indexBuffer);
		auto vb = vertexBuffers.at(mesh);
		device->UpdateBufferData(vb, mesh->vertexBuffer);
	}

	void SceneRender::AddCamera(Camera* cam) { cameras.push_back(cam); }

	Camera& SceneRender::GetCamera(size_t idx) { return *cameras[idx]; }

	Material& SceneRender::CreateMaterial(Shader* sh) {
		auto mat = new Material({}, *device, *sh);
		materials.push_back(mat);
		return *mat;
	}
}