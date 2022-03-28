#pragma once
#include "../../stdafx.h"
#include "../../Device.h"
#include "Canvas.h"

namespace Game::UI {
	__interface IDrawable {
		void SetCanvas(UI::Canvas* canvas);
		void Draw(Device* device);
	};
}
