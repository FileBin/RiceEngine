#include "../stdafx.hpp"
NSP_ENGINE_BEGIN
struct IRenderable;
typedef RefPtr<IRenderable> pIRenderable;
NSP_ENGINE_END
#pragma once

#include "../Math.hpp"
//#include "../Util/IRegistrable.hpp"

NSP_ENGINE_BEGIN

struct IRenderable {
	virtual void render(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP) = 0;
	virtual void renderTransparent(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP) = 0;
	virtual ~IRenderable() {}
};

NSP_ENGINE_END
