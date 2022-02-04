#pragma once

#include "../Matrixes.h"
#include "../Util/IRegistrable.h"

namespace Game {
	__interface IRenderable : public IRegistrable {
		void Render(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP);
		void RenderTransparent(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP);
	};
}