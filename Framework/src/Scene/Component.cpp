#include <GameEngine\Scene\Component.h>
#include <GameEngine\Scene\SceneObject.h>
namespace Game {
	void Component::Init(SceneObject* parent) {
		obj = parent; 
		OnInit();
	}
	Component::~Component() {};
	SceneObject& Component::GetSceneObject() { return *obj; }
}