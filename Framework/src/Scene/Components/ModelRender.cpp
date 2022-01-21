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
		if (!model.IsNull()) {
			ren.AddModel(this, &transform);
		}
		enabled = true;
	}

	void ModelRender::OnDisable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		if (!model.IsNull()) {
 			ren.RemoveModel(model.Get());
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
			if (model.Get()) {
				ren.ChangeModel(this, &transform, model.Get());
			} else {
				ren.AddModel(this, &transform);
			}
		}
		model = _model;
	}
	SmartPtr<Model> ModelRender::GetModel() const { return model; }
	SmartPtr<Material> ModelRender::GetMaterial(size_t subMeshIdx) const { return materials[subMeshIdx]; }
}