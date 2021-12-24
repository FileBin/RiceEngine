#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Scene\SceneObject.h>
#include <GameEngine\Scene\Scene.h>

namespace Game {
	void ModelRender::OnInit() { model = nullptr; }

	void ModelRender::Enable() {
		auto& ren = GetSceneObject().GetScene().GetRender();
		if (model != nullptr) {
			ren.AddModel(model);
			auto n = model->GetSubMeshesCount();
			for (size_t i = 0; i < n; i++) {
				auto& mesh = model->GetSubMesh(i);
				ren.MapMaterial(&mesh, materials[i]);
			}
		}
		enabled = true;
	}

	void ModelRender::Disable() {
		enabled = false;
		auto& ren = GetSceneObject().GetScene().GetRender();
		if (model) {
			ren.RemoveModel(model);
			model = nullptr;
		}
	}

	void ModelRender::SetMaterial(Material* material, size_t i) { materials[i] = material; }

	void ModelRender::SetModel(Model* model, bool updateBuffer) {
		auto& ren = GetSceneObject().GetScene().GetRender();

		auto n = model->GetSubMeshesCount();
		materials.resize(n);
		if (model)
			ren.RemoveModel(model);
		this->model = model;
		if (enabled) {
			ren.AddModel(model);
			for (size_t i = 0; i < n; i++) {
				auto& mesh = this->model->GetSubMesh(i);
				ren.MapMaterial(&mesh, materials[i]);
			}
			//ren.UpdateModel(this->model);
		}
	}
	Model& ModelRender::GetModel() const { return *model; }
	Material& ModelRender::GetMaterial(size_t subMeshIdx) const { return *materials[subMeshIdx]; }
}