#include "../../stdafx.hpp"
NSP_UI_BEGIN

interface IDrawable;
typedef SmartPtr<IDrawable> pIDrawable;

NSP_UI_END
#pragma once
#include "Canvas.hpp"

NSP_UI_BEGIN

interface IDrawable {
	virtual void SetCanvas(Canvas* canvas) = 0;
	//void Draw(Device* device);
	virtual ~IDrawable() {};
};

NSP_UI_END
