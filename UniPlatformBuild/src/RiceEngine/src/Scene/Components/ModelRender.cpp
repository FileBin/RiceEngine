#include "pch.h"
#include <GameEngine\Components\Transform.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	void ModelRender::OnInit() { 
		model = nullptr; 
		device = GetSceneObject().GetScene().GetEngine().GetDevice();
	}

	void ModelRender::OnEnable() {
		auto& scene = GetSceneObject().GetScene();
		auto& ren = scene.GetRender();
		transform = GetSceneObject().GetComponents<Transform>()[0];

		ren.RegisterModel(this);

		enabled = true;
	}

	void ModelRender::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		ren.UnregisterModel(this);
		enabled = false;
	}

	void ModelRender::SetMaterial(SmartPtr<Material> material, size_t i) {
		std::lock_guard l(renderMutex);
		meshes[i].material = material;
	}

	void ModelRender::SetModel(SmartPtr<Model> _model) {
		if (model == _model) return;
		auto n = _model->GetSubMeshesCount();

		std::lock_guard l(renderMutex);
		meshes.resize(n);

		for (size_t i = 0; i < n; i++) {
			auto mesh = _model->GetSubMesh(i);

			RenderData& rm = meshes[i];

			if (mesh->indexBuffer.empty()) {
				_RELEASE(rm.indexBuffer);
				_RELEASE(rm.vertexBuffer);
				continue;
			}
			rm.indexBuffer = device->CreateBuffer<UINT>(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
			rm.vertexBuffer = device->CreateBuffer<Vertex>(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		}
		std::unique_lock lock(modelMutex);
		model = _model;
	}
	SmartPtr<Model> ModelRender::GetModel() {
		std::shared_lock l(modelMutex);
		return model; 
	}

	SmartPtr<Material> ModelRender::GetMaterial(size_t subMeshIdx) const { return meshes[subMeshIdx].material; }

	void ModelRender::Render(Matrix4x4& View, Matrix4x4& Projection, Matrix4x4& LVP, RenderType type) {
		if (model.IsNull()) return;
		ConstantBufferData cb = {};
		auto World = transform->GetTransformationMatrix();
		cb.World = World;
		cb.WorldView = cb.World * View;
		cb.Projection = Projection;
		cb.LightWVP = World * LVP;
		auto n = meshes.size();
		auto cBuffer = GetSceneObject().GetScene().GetRender().GetConstBuffer();

		device->LoadBufferSubresource(cBuffer, cb);

		std::lock_guard l(renderMutex);
		for (size_t i = 0; i < n; i++) {
			const auto& mesh = meshes[i];
			modelMutex.lock_shared();
			auto m = model->GetSubMesh(i);
			modelMutex.unlock_shared();
			//if (!m->CheckVisiblity(cb)) continue;
			if (mesh.material.Get() && mesh.indexBuffer.Get() && m->CheckVisiblity(cb)) {
				if (mesh.material->renderType == type) {
					setActiveMaterial(mesh.material);
					device->SetActiveVertexBuffer<Vertex>(mesh.vertexBuffer.Get());
					device->SetActiveIndexBuffer(mesh.indexBuffer.Get());

					device->Draw();
				}
			}
		}
	}

	void ModelRender::Render(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP) {
		Render(View, Projection, LVP, RenderType::Solid);
	}

	void ModelRender::RenderTransparent(Matrix4x4 View, Matrix4x4 Projection, Matrix4x4 LVP) {
		Render(View, Projection, LVP, RenderType::Transparent);
	}

	void ModelRender::Register(size_t i) { idx = i; }

	size_t ModelRender::Unregister() { 
		auto i = idx;
		idx = SIZE_T_MAX;
		return i;
	}

	void ModelRender::setActiveMaterial(SmartPtr<Material> pMat) {
		device->SetActiveVSConstantBuffer(pMat->GetBuffer(), 1);
		device->SetActivePSConstantBuffer(pMat->GetBuffer());
		device->SetPSTextures(pMat->GetTextures());
		device->SetActiveShader(pMat->GetShader());
	}



}