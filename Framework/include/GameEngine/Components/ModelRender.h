#pragma once

#include "../Scene/Component.h"
#include <vector>

namespace Game {

	using std::vector;

	class Model;
	class Material;

	class ModelRender : public Component {
	public:
		void OnInit();

		void OnEnable();
		void OnDisable();

		void SetMaterial(Material* material, size_t subMeshIdx);
		void SetModel(Model* model, bool updateBuffer = true);
		Model& GetModel() const;
		Material& GetMaterial(size_t subMeshIdx) const;
	private:
		Model* model = nullptr;
		vector<Material*> materials{};
	};
}
