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

		void DeleteModel() { model = nullptr; }

		void SetMaterial(std::shared_ptr<Material> material, size_t subMeshIdx);
		void SetModel(std::shared_ptr<Model> model, bool updateBuffer = true);
		Model& GetModel() const;
		Material& GetMaterial(size_t subMeshIdx) const;
	private:
		std::shared_ptr<Model> model = nullptr;
		concurrency::concurrent_vector<std::shared_ptr<Material>> materials{};
	};
}
