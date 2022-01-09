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
			ren.AddModel(model, materials);
		}
		enabled = true;
	}

	void ModelRender::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		if (!model.IsNull()) {
			model->pPos = nullptr;
			model->pRot = nullptr;
			model->pScale = nullptr;
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
		if (enabled) {
			_model->pPos = &transform.position;
			_model->pRot = &transform.rotation;
			_model->pScale = &transform.scale;
			if (model.Get()) {
				ren.ChangeModel(model, _model, materials);
			} else {
				ren.AddModel(_model, materials);
			}
		}
		model = _model;
	}
	SmartPtr<Model> ModelRender::GetModel() const { return model; }
	SmartPtr<Material> ModelRender::GetMaterial(size_t subMeshIdx) const { return materials[subMeshIdx]; }
}