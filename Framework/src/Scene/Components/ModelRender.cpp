#include "pch.h"
#include <GameEngine\Components\Transform.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	void ModelRender::OnInit() { model = nullptr; }

	void ModelRender::OnEnable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		auto& transform = *GetSceneObject().GetComponents<Transform>()[0];

		for (auto& rm : renderingMeshes) {
			if (rm->mat.IsNull()) continue;
			ren.AddModel(reinterpret_cast<SmartPtr<IRenderable>&>(rm));
		}

		enabled = true;
	}

	void ModelRender::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		for (auto& rm : renderingMeshes) {
			ren.RemoveModel(reinterpret_cast<SmartPtr<IRenderable>&>(rm));
		}
		enabled = false;
	}

	void ModelRender::SetMaterial(SmartPtr<Material> material, size_t i) {
		renderingMeshes[i]->mat = material; 
	}

	void ModelRender::SetModel(SmartPtr<Model> _model) {
		if (model == _model) return;
		auto& en = GetSceneObject().GetScene().GetEngine();
		auto& ren = GetSceneObject().GetScene().GetRender();
		auto& transform = *GetSceneObject().GetComponents<Transform>()[0];
		auto beg = renderingMeshes.size();
		auto n = _model->GetSubMeshesCount();
		renderingMeshes.resize(n);
		for (size_t i = 0; i < n; i++) {
			auto mesh = _model->GetSubMesh(i);
			auto& rm = renderingMeshes[i];
			if (rm.IsNull()) rm = new RenderingMesh();
			std::lock_guard l(rm->renderMutex);
			rm->device = en.GetDevice();
			rm->orig = mesh;
			rm->pConstBuffer = ren.GetConstBuffer();

			rm->transform = &transform;

			if (rm->pIndexBuffer.Get()) {
				rm->pIndexBuffer->Release();
				rm->pIndexBuffer = nullptr;
			}
			if (rm->pVertexBuffer.Get()) {
				rm->pVertexBuffer->Release();
				rm->pVertexBuffer = nullptr;
			}

			if (mesh->indexBuffer.empty()) continue;

			rm->pIndexBuffer = en.GetDevice()->CreateBuffer<UINT>(mesh->indexBuffer, D3D11_BIND_INDEX_BUFFER);
			rm->pVertexBuffer = en.GetDevice()->CreateBuffer<Vertex>(mesh->vertexBuffer, D3D11_BIND_VERTEX_BUFFER);
		}
		if (enabled) {
			for (auto i = beg; i < n; i++) {
				auto& rm = renderingMeshes[i];
				if (rm->mat.IsNull()) continue;
				ren.AddModel(reinterpret_cast<SmartPtr<IRenderable>&>(rm));
			}
		}
		model = _model;
	}
	SmartPtr<Model> ModelRender::GetModel() const { return model; }
	SmartPtr<Material> ModelRender::GetMaterial(size_t subMeshIdx) const { return renderingMeshes[subMeshIdx]->mat; }
}