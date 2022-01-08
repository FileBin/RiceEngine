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
		model->pPos = &transform.position;
		model->pRot = &transform.rotation;
		model->pScale = &transform.scale;
		if (!model.IsNull()) {
			ren.AddModel(model);
			auto n = model->GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				auto mesh = model->GetSubMesh(i);
				ren.MapMaterial(mesh, materials[i]);
			}
		}
		enabled = true;
	}

	void ModelRender::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		if (!model.IsNull()) {
			model->pPos = nullptr;
			model->pRot = nullptr;
			model->pScale = nullptr;
			auto n = model->GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				auto mesh = model->GetSubMesh(i);
				ren.UnmapMaterial(mesh);
			}
 			ren.RemoveModel(model);
		}
		enabled = false;
	}

	void ModelRender::SetMaterial(SmartPtr<Material> material, size_t i) {
		materials[i] = material; 
	}

	void ModelRender::SetModel(SmartPtr<Model> _model, bool updateBuffer) {
		if (model == _model) return;
		auto& ren = GetSceneObject().GetScene().GetRender();
		auto& transform = *GetSceneObject().GetComponents<Transform>()[0];

		auto n = _model->GetSubMeshesCount();
		materials.resize(n);
		if (!model.IsNull()) {
			if (enabled) {
				model->pPos = nullptr;
				model->pRot = nullptr;
				model->pScale = nullptr;
				auto n = model->GetSubMeshesCount();
				for (size_t i = 0; i < n; i++) {
					auto mesh = model->GetSubMesh(i);
					ren.UnmapMaterial(mesh);
				}
				ren.RemoveModel(model);
			}
			//model.Release();
		}
		model = _model;
		if (enabled) {
			model->pPos = &transform.position;
			model->pRot = &transform.rotation;
			model->pScale = &transform.scale;
			ren.AddModel(model);
			for (size_t i = 0; i < n; i++) {
				auto mesh = this->model->GetSubMesh(i);
				ren.MapMaterial(mesh, materials[i]);
			}
			//ren.UpdateModel(this->model);
		}
	}
	SmartPtr<Model> ModelRender::GetModel() const { return model; }
	SmartPtr<Material> ModelRender::GetMaterial(size_t subMeshIdx) const { return materials[subMeshIdx]; }
}