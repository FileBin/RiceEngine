#pragma once

#include "../Matrixes.h"

namespace Game {
	__interface IRenderable {
		void Render(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP);
		bool IsTransparent();
	};
}