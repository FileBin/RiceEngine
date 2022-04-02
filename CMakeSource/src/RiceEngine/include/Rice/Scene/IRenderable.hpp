#include "../stdafx.hpp"
NSP_ENGINE_BEGIN
interface IRenderable;
typedef SmartPtr<IRenderable> pIRenderable;
NSP_ENGINE_END
#pragma once

#include "../Math.hpp"
#include "../Util/IRegistrable.hpp"

NSP_ENGINE_BEGIN

interface IRenderable: public IRegistrable {
	virtual void Render(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP) = 0;
	virtual void RenderTransparent(Matrix4x4 View, Matrix4x4 Project, Matrix4x4 LVP) = 0;
	virtual ~IRenderable() {}
};

NSP_ENGINE_END
