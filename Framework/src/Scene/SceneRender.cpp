#include "pch.h"
#include <GameEngine\SceneRender.h>
#include <GameEngine\Material.h>
#include <GameEngine\Model.h>
#include <GameEngine\Camera.h>
#include <GameEngine\Util\exceptions.h>
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

	void SceneRender::RenderingMesh::Draw(SceneRender* ren, Camera& cam) {
		auto device = ren->device;
		auto constantBuffer = ren->constantBuffer.Get();
		ConstantBufferData cb = {};
		cb.World = Matrix4x4::TRS(*pPos, *pRot, *pScale);
		cb.View = cam.GetTransformationMatrix();
		cb.Projection = cam.GetProjectionMatrix();
		//cb.World.c41 = clock;

		device->LoadBufferSubresource(constantBuffer, cb);
		device->SetActiveVSConstantBuffer(constantBuffer);

		device->SetActiveVertexBuffer<Vertex>(pVertexBuffer.Get());
		device->SetActiveIndexBuffer(pIndexBuffer.Get());
		device->SetActivePSConstantBuffer(pMat->GetBuffer());
		device->SetActiveShader(pMat->GetShader());
		device->SetPSTextures(pMat->GetTextures());

		device->Draw();
	}

	bool SceneRender::Draw() {
		std::unique_ptr<std::lock_guard<std::mutex>> locks[0x100];
		auto cam = *cameras[activeCameraIdx];
		queue<pair<SmartPtr<Mesh>, ConstantBufferData>> transparentQ;
		device->SetPrimitiveTopology();
		device->SetBlendState(false);
		//float time = clock() * .001;
		m_mutex.lock();
		for (auto& pair : renderingMeshes) {
			pair.second->Draw(this, cam);
		}

		device->SetBlendState(true);
		device->UnsetDepthBuffer();
		for (auto& pair : transparentMeshes) {
			pair.second->Draw(this, cam);
		}
		m_mutex.unlock();

		device->Begin2D();
		m_2dMutex.lock();
		for (auto& d : drawables) {
			d->Draw(device->GetWriteFactory(), device->GetDefFormat(), device->Get2DRenderTarget());
		}
		m_2dMutex.unlock();
		device->End2D();

		return true;
	}

	void SceneRender::Close() {
		for (auto& m : shaders) { m.second.Release(); }
		for (auto& m : materials) { m.second.Release(); }
		_RELEASE(constantBuffer);
	}

	void SceneRender::Resize() {
		auto n = cameras.size();
		auto ar = GetAspectRatio();
		for (size_t i = 0; i < n; i++) {
			cameras[i]->SetAspectRatio(ar);
		}
	}

	void SceneRender::AddModel(SmartPtr<Model> model, std::vector<SmartPtr<Material>> materials) {
		auto n = model->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = model->GetSubMesh(i);
			if (mesh->vertexBuffer.empty()) continue;
			auto rMesh = new RenderingMesh();
			rMesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
			rMesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
			rMesh->orig = mesh;

			rMesh->pPos = model->pPos;
			rMesh->pRot = model->pRot;
			rMesh->pScale = model->pScale;

			rMesh->pMat = materials[i];

			std::lock_guard lock(m_mutex);

			switch (rMesh->pMat->renderType) {
			case RenderType::Solid:
				renderingMeshes[mesh] = rMesh;
				break;
			case RenderType::Transparent:
				transparentMeshes[mesh] = rMesh;
				break;
			default:
				THROW_INVALID_ARG_EXCEPTION("material->rederType");
				break;
			}
		}
	}

	void SceneRender::RemoveModel(SmartPtr<Model> model) {
		auto n = model->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = model->GetSubMesh(i);
			std::lock_guard lock(m_mutex);
			auto it = renderingMeshes.find(mesh);
			if (it != renderingMeshes.end()) {
				it->second.Release();
				renderingMeshes.erase(it);
				continue;
			} 
			it = transparentMeshes.find(mesh);
			if (it != transparentMeshes.end()) {
				it->second.Release();
				transparentMeshes.erase(it);
			}
		}
	}

	void SceneRender::ChangeModel(SmartPtr<Model> removemodel, SmartPtr<Model> replace, std::vector<SmartPtr<Material>> materials) {
		std::lock_guard lock(m_mutex);
		auto n = removemodel->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = removemodel->GetSubMesh(i);
			auto it = renderingMeshes.find(mesh);
			if (it != renderingMeshes.end()) {
				it->second.Release();
				renderingMeshes.erase(it);
				continue;
			}
			it = transparentMeshes.find(mesh);
			if (it != transparentMeshes.end()) {
				it->second.Release();
				transparentMeshes.erase(it);
			}
		}
		n = replace->GetSubMeshesCount();
		for (size_t i = 0; i < n; i++) {
			auto mesh = replace->GetSubMesh(i);
			if (mesh->vertexBuffer.empty()) continue;
			auto rMesh = new RenderingMesh();
			rMesh->pVertexBuffer = device->CreateBuffer(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
			rMesh->pIndexBuffer = device->CreateBuffer(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
			rMesh->orig = mesh;

			rMesh->pPos = replace->pPos;
			rMesh->pRot = replace->pRot;
			rMesh->pScale = replace->pScale;

			rMesh->pMat = materials[i];

			switch (rMesh->pMat->renderType) {
			case RenderType::Solid:
				renderingMeshes[mesh] = rMesh;
				break;
			case RenderType::Transparent:
				transparentMeshes[mesh] = rMesh;
				break;
			default:
				THROW_INVALID_ARG_EXCEPTION("material->rederType");
				break;
			}
		}
	}

	bool SceneRender::UpdateBuffers(SmartPtr<Mesh> mesh) {

		auto it = renderingMeshes.find(mesh);
		if (it == renderingMeshes.end()) {
			it = transparentMeshes.find(mesh);
			if (it == transparentMeshes.end())
				return false;
		}
		std::lock_guard lock(m_mutex);
		auto& rMesh = *it->second;
		device->UpdateBufferData(rMesh.pIndexBuffer.Get(), mesh->indexBuffer);
		device->UpdateBufferData(rMesh.pVertexBuffer.Get(), mesh->vertexBuffer);
		return true;
	}

	void SceneRender::AddCamera(SmartPtr<Camera> cam) { cameras.push_back(cam); }

	SmartPtr<Camera> SceneRender::GetCamera(size_t idx) { return { cameras[idx] }; }

	SmartPtr<Shader> SceneRender::CreateShader(String name) {
		auto sh = new Shader(device);
		shaders.insert({name, sh});
		return sh;
	}

	SmartPtr<Shader> SceneRender::GetShader(String name) {
		try {
			return shaders.at(name);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION("name");
		}
	}

	SmartPtr<Material> SceneRender::CreateMaterial(String name, SmartPtr<Shader> sh, std::vector<std::pair<String, size_t>> mapping) {
		SmartPtr<Material> mat = new Material(*device, *sh, mapping);
		materials.insert(materials.end(), { name, mat });
		return mat;
	}
	SmartPtr<Material> SceneRender::GetMaterial(String name) {
		try {
			return materials.at(name);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION("name");
		}
	}
	void SceneRender::AddDrawable(UI::IDrawable* txt) {
		std::lock_guard lock(m_2dMutex);
		drawables.push_back(txt);
	}
	void SceneRender::RemoveDrawable(UI::IDrawable* txt) {
		for (auto it = drawables.begin(); it != drawables.end(); it++) {
			if (it->Get() == txt) {
				std::lock_guard lock(m_2dMutex);
				drawables.erase(it);
				break;
			}
		}
	}
}