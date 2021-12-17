#pragma once

#include "../Scene/Component.h"
#include <vector>

namespace Game {
	class Model;
	class Material;

	class ModelRender : public Component {
	public:
		void OnInit();

		void Enable();

		void SetMaterial(Material* material, size_t subMeshIdx);
		void SetModel(Model* model);
		Model& GetModel() const;
		Material& GetMaterial(size_t subMeshIdx) const;

	private:
		Model* model;
		std::vector<Material*> materials;
	};
}
