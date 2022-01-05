#include "pch.h"
#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>
#include <GameEngine\Util\exception.h>
#include <queue>
#include <GameEngine\Components\UI\IDrawable.h>

namespace Game {
	using std::queue;
	bool SceneRender::Init() {
		constantBuffer = device->CreateBuffer<ConstantBufferData>({}, D3D11_BIND_CONSTANT_BUFFER);
		return true;
	}
	void SceneRender::BeginFrame() {
		device->ClearFrame({ 0.1f, 0.15f, 0.6f, 1.f });
	}
	bool SceneRender::Draw() {
		while (CheckLoading())
			Sleep(1);

		isRendering = true;
		device->SetPrimitiveTopology();
		device->SetBlendState(false);
		auto n = models.size();
		float time = clock() * .001;
		for (auto it = models.begin(); it != models.end(); it++) {
			auto model = it->first;

			ConstantBufferData cb = {};

			auto cam = cameras[activeCameraIdx];
			cb.World = Matrix4x4::TRS(*model->pPos, *model->pRot, *model->pScale);
			cb.View = cam->GetTransformationMatrix();
			cb.Projection = cam->GetProjectionMatrix();
			cb.World.c41 = time;


			device->LoadBufferSubresource(constantBuffer, cb);
			device->SetActiveVSConstantBuffer(constantBuffer);

			auto m = model->GetSubMeshesCount();
			for (size_t j = 0; j < m; j++) {
				auto mesh = model->GetSubMesh(j).lock();

				auto matIt = materialMap.find(mesh.get());
				if (matIt == materialMap.end()) continue;
				auto mat = matIt->second.lock();

				if (mat == nullptr) continue;

				if (mat->renderType == RenderType::Transparent) {
					auto i = transparentQ.find(mesh.get());
					if (i != transparentQ.end()) {
						i->second = cb;
					}
					continue;
				}

				if (!mesh->CheckVisiblity(cb)) continue;

				Buffer *ib, *vb;
				auto iIt = indexBuffers.find(mesh.get());
				if (iIt != indexBuffers.end()) {
					ib = iIt->second;
					vb = vertexBuffers.at(mesh.get());
				} else {
					continue;
				}

				device->SetActiveVertexBuffer<Vertex>(vb);
				device->SetActiveIndexBuffer(ib);

				device->SetActivePSConstantBuffer(mat->GetBuffer());
				device->SetActiveShader(mat->GetShader());
				device->SetPSTextures(mat->GetTextures());
				
				device->Draw();
			}
		}
		device->SetBlendState(true);
		device->UnsetDepthBuffer();
		for (auto pair = transparentQ.begin(); pair != transparentQ.end(); pair++) {
			auto m = pair->first;

			if (!m->CheckVisiblity(pair->second)) continue;

			device->LoadBufferSubresource(constantBuffer, pair->second);
			device->SetActiveVSConstantBuffer(constantBuffer);

			Buffer* ib, * vb;
			auto iIt = indexBuffers.find(m);
			if (iIt != indexBuffers.end()) {
				ib = iIt->second;
				vb = vertexBuffers.at(m);
			} else {
				continue;
			}

			device->SetActiveVertexBuffer<Vertex>(vb);
			device->SetActiveIndexBuffer(ib);

			auto matIt = materialMap.find(m);
			if (matIt == materialMap.end()) continue;
			auto mat = matIt->second.lock();
			device->SetActivePSConstantBuffer(mat->GetBuffer());
			device->SetActiveShader(mat->GetShader());
			device->SetPSTextures(mat->GetTextures());

			device->Draw();
		}

		device->Begin2D();

		for (auto pair : texts) {
			auto& d = pair.first;
			d->Draw(device->GetWriteFactory(), device->GetDefFormat(), device->Get2DRenderTarget());
		}
		device->End2D();

		isRendering = false;
		return true;
	}

	void SceneRender::Close() {
		for (auto c : cameras) delete c;
		for (auto m : shaders) { delete m.second; }
		for (auto m : indexBuffers) { _RELEASE(m.second); }
		for (auto m : vertexBuffers) { _RELEASE(m.second); }
		_RELEASE(constantBuffer);
	}

	void SceneRender::Resize() {
		auto n = cameras.size();
		auto ar = GetAspectRatio();
		for (size_t i = 0; i < n; i++) {
			cameras[i]->SetAspectRatio(ar);
		}
	}

	void SceneRender::AddModel(std::weak_ptr<Model> model) {
		auto mod = model.lock();
		auto n = mod->GetSubMeshesCount();

		bool b = false;

		for (size_t i = 0; i < n; i++) {
			auto mesh = mod->GetSubMesh(i).lock();
			if (mesh->indexBuffer.size() == 0) continue;
			b = true;
			auto vertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto indexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE);

			vertexBuffers.insert(vertexBuffers.end(), { mesh.get(), vertexBuffer });
			indexBuffers.insert(indexBuffers.end(), { mesh.get(), indexBuffer });
		}
		if (b) {
			models.insert(models.end(), { mod.get(), true });
		}
	}

	bool SceneRender::RemoveModel(std::weak_ptr<Model> model, bool del) {
		auto it = models.find(model.lock().get());
		if (it != models.end()) {
			//Wait();
			//Lock();
			auto model = it->first;
			models.unsafe_erase(it);
			auto n = model->GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				auto m = model->GetSubMesh(i).lock();
				auto vIt = vertexBuffers.find(m.get());
				if (vIt != vertexBuffers.end()) {
					vIt->second->Release();
					vertexBuffers.unsafe_erase(vIt);
				}
				auto iIt = indexBuffers.find(m.get());
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

	void SceneRender::UpdateModel(std::weak_ptr<Model> model) {
		auto mod = model.lock();
		auto n = mod->GetSubMeshesCount();

		for (size_t i = 0; i < n; i++) {
			auto mesh = mod->GetSubMesh(i).lock();

			auto vertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto indexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
			auto it = vertexBuffers.find(mesh.get());
			if (it != vertexBuffers.end()) {
				it->second = vertexBuffer;
			} else {
				vertexBuffers.insert(vertexBuffers.end(), { mesh.get(), vertexBuffer });
			}
			it = indexBuffers.find(mesh.get());
			if (it != indexBuffers.end()) {
				it->second = indexBuffer;
			} else {
				indexBuffers.insert(indexBuffers.end(), { mesh.get(), indexBuffer });
			}
		}
	}



	void SceneRender::MapMaterial(std::weak_ptr<Mesh> mesh, std::weak_ptr<Material> mat) {
		auto msh = mesh.lock();
		auto m = mat.lock();
		if (!m.get())
			return;
		if (m->renderType == RenderType::Transparent)
			transparentQ.insert({ msh.get(), {} });
		materialMap.insert({ msh.get(), mat});
	}

	void SceneRender::UnmapMaterial(std::weak_ptr<Mesh> mesh) {
		auto msh = mesh.lock();
		auto it = materialMap.find(msh.get());
		if (it != materialMap.end()) {
			if (it->second.lock()->renderType == RenderType::Transparent)
				transparentQ.unsafe_erase(msh.get());
			materialMap.unsafe_erase(it);
		}
	}

	void SceneRender::UpdateBuffer(std::weak_ptr<Mesh> mesh) {
		auto msh = mesh.lock();
		auto ib = indexBuffers.at(msh.get());
		device->UpdateBufferData(ib, msh->indexBuffer);
		auto vb = vertexBuffers.at(msh.get());
		device->UpdateBufferData(vb, msh->vertexBuffer);
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

	shared_ptr<Material> SceneRender::CreateMaterial(String name, Shader* sh, std::vector<std::pair<String, size_t>> mapping) {
		auto mat = std::make_shared<Material>(*device, *sh, mapping);
		materials.insert(materials.end(), { name, mat });
		return mat;
	}
	shared_ptr<Material> SceneRender::GetMaterial(String name) {
		auto it = materials.find(name);
		if (it != materials.end()) {
			return it->second;
		}
		throw new Game::exception("Material name invalid", 257, L"SceneRender.cpp : Material& SceneRender::GetMaterial(String name)");
	}
	void SceneRender::AddDrawable(UI::IDrawable* txt) {
		texts.insert({ txt, true });
	}
	void SceneRender::RemoveDrawable(UI::IDrawable* txt) {
		auto it = texts.find(txt);
		if (it != texts.end()) {
			texts.unsafe_erase(it);
		}
	}
}