#pragma once
#include "Component.h"
#include <rapidjson\document.h>

namespace Game {
	class ComponentBuilder {
		typedef _STD function<Component* (rapidjson::GenericObject<true, rapidjson::Value>)> BuliderFunc;
		static _STD unordered_map<_STD string, BuliderFunc> builders;
	};
	class ObjectSerializer {
	private:

	public:
		static void LoadFromJSON(String json_path, SceneObject* obj);
	};
}
