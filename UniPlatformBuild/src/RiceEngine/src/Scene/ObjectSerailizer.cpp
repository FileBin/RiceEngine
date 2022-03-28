#include "pch.h"
#include <GameEngine\Scene\ObjectSerializer.h>
#include <GameEngine\Scene\SceneObject.h>
namespace Game {
	using namespace std;
	unordered_map<string, ComponentBuilder::BuliderFunc> ComponentBuilder::builders{

	};
	void ObjectSerializer::LoadFromJSON(String json_path, SceneObject* obj) {
	}
}