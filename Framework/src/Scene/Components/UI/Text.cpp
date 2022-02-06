#include "pch.h"
#include <GameEngine/Components/UI/Text.h>
#include <GameEngine/Util.h>
#include <GameEngine/Scene/Scene.h>

using namespace Game::Util;

namespace Game::UI {

	void Text::Draw(Device* device) {
		drawer.Draw(device, canvas, transform->GetPosition2DWithAnchor(canvas), transform->GetReferenceScale2D());
	}
}