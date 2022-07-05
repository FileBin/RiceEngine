#include "../../stdafx.hpp"
NSP_UI_BEGIN

struct IDrawable;
typedef RefPtr<IDrawable> pIDrawable;

NSP_UI_END
#pragma once
#include "Canvas.hpp"

NSP_UI_BEGIN

struct IDrawable {
    virtual void setCanvas(pCanvas canvas) = 0;
	//void Draw(Device* device);
	virtual ~IDrawable() {};
};

NSP_UI_END
