#include "pch.h"
#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>
#include <GameEngine\Util\exception.h>
#include <queue>

namespace Game {
	using std::queue;
	bool SceneRender::Init() {
		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);
		device->CreateFonts();
		return true;
	}
	void SceneRender::BeginFrame() {
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
	}
	bool SceneRender::Draw() {
		while (isLoading)
			Sleep(1);

		isRendering = true;
		device->SetPrimitiveTopology();
		device->SetBlendState(false);
		for (auto it = models.begin(); it != models.end(); it++) {
			auto& model = it->first;

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

				auto matIt = materialMap.find(&mesh);
				if (matIt == materialMap.end()) continue;
				auto mat = matIt->second;

				if (mat == nullptr) continue;

				if (mat->renderType == RenderType::Transparent) {
					auto i = transparentQ.find(&mesh);
					if (i != transparentQ.end()) {
						i->second = cb;
					}
					continue;
				}

				if (!mesh.CheckVisiblity(cb)) continue;

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

				device->SetActivePSConstantBuffer(mat->GetBuffer());
				device->SetActiveShader(mat->GetShader());
				
				device->Draw();
			}
		}
		device->SetBlendState(true);
		for (auto pair = transparentQ.begin(); pair != transparentQ.end(); pair++) {
			auto& m = *pair->first;

			if (!m.CheckVisiblity(pair->second)) continue;

			device->LoadBufferSubresource(constantBuffer, pair->second);
			device->SetActiveVSConstantBuffer(constantBuffer);

			Buffer* ib, * vb;
			auto iIt = indexBuffers.find(&m);
			if (iIt != indexBuffers.end()) {
				ib = iIt->second;
				vb = vertexBuffers.at(&m);
			} else {
				continue;
			}

			device->SetActiveVertexBuffer<Vertex>(vb);
			device->SetActiveIndexBuffer(ib);

			auto matIt = materialMap.find(&m);
			if (matIt == materialMap.end()) continue;
			auto& mat = matIt->second;
			device->SetActivePSConstantBuffer(mat->GetBuffer());
			device->SetActiveShader(mat->GetShader());

			device->Draw();
		}
		//device->Draw2D();
		isRendering = false;
		return true;
	}

	void SceneRender::Close() {
		for (auto c : cameras) delete c;
		for (auto m : materials) { delete m.second; }
		for (auto m : shaders) { delete m.second; }
		for (auto m : indexBuffers) { _RELEASE(m.second); }
		for (auto m : vertexBuffers) { _RELEASE(m.second); }
		for (auto m : models) { delete m.first; }
		_RELEASE(constantBuffer);
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
			models.insert(models.end(), { model, true });
		}
	}

	bool SceneRender::RemoveModel(Model* model, bool del) {
		auto it = models.find(model);
		if (it != models.end()) {
			//Wait();
			//Lock();
			auto& model = *it->first;
			models.unsafe_erase(it);
			auto n = model.GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				auto m = &model.GetSubMesh(i);
				auto vIt = vertexBuffers.find(m);
				if (vIt != vertexBuffers.end()) {
					vIt->second->Release();
					vertexBuffers.unsafe_erase(vIt);
				}
				auto iIt = indexBuffers.find(m);
				if (iIt != indexBuffers.end()) {
					iIt->second->Release();
					indexBuffers.unsafe_erase(iIt);
				}
			}
			//Unlock();
			return true;
		}
		return false;
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



	void SceneRender::MapMaterial(Mesh* mesh, Material* mat) {
		if (mat == nullptr)
			return;
		if (mat->renderType == RenderType::Transparent)
			transparentQ.insert({ mesh, {} });
		materialMap.insert(materialMap.end(), {mesh, mat}); 
	}

	void SceneRender::UnmapMaterial(Mesh* mesh) {
		while (isRendering)
			Sleep(1);
		auto it = materialMap.find(mesh);
		if (it != materialMap.end()) {
			if (it->second->renderType == RenderType::Transparent)
				transparentQ.unsafe_erase(mesh);
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

	Shader* SceneRender::CreateShader(String name) {
		auto sh = new Shader(device);
		shaders.insert({name, sh});
		return sh;
	}

	Shader& SceneRender::GetShader(String name) {
		auto it = shaders.find(name);
		if (it != shaders.end()) {
			return *it->second;
		}
		throw new Game::exception("Shader name invalid", 244, L"SceneRender.cpp : Shader& SceneRender::GetShader(String name)");
	}

	Material& SceneRender::CreateMaterial(String name, Shader* sh, std::vector<std::pair<String, size_t>> mapping) {
		auto mat = new Material(*device, *sh, mapping);
		materials.insert(materials.end(), { name, mat });
		return *mat;
	}
	Material& SceneRender::GetMaterial(String name) {
		auto it = materials.find(name);
		if (it != materials.end()) {
			return *it->second;
		}
		throw new Game::exception("Material name invalid", 257, L"SceneRender.cpp : Material& SceneRender::GetMaterial(String name)");
	}
}