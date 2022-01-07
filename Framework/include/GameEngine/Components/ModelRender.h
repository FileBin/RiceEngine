#pragma once

#include "../Scene/Component.h"
#include <GameEngine/macros.h>
#include "../Model.h"
#include <concurrent_vector.h>

namespace Game {

	using std::vector;

	class Material;

	class ModelRender : public Component {
	public:
		void OnInit();

		void OnEnable();
		void OnDisable();

		void DeleteModel() { model.Release(); }

		void SetMaterial(SmartPtr<Material> material, size_t subMeshIdx);
		void SetModel(SmartPtr<Model> model, bool updateBuffer = true);
		SmartPtr<Model> GetModel() const;
		SmartPtr<Material> GetMaterial(size_t subMeshIdx) const;
	private:
		SmartPtr<Model> model;
		concurrency::concurrent_vector<SmartPtr<Material>> materials{};
	};
}
