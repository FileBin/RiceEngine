#pragma once
#include "../../stdafx.h"

namespace Game::UI {
	__interface IDrawable {
		void Draw(IDWriteFactory* factory, IDWriteTextFormat* defformat, ID2D1RenderTarget* rt);
	};
}
