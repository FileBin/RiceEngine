#pragma once
#include "Core.h"
#include "ScriptBase.h"
#include "SceneRender.h"

using namespace Game;

class RenderScript : public ScriptBase {
public:
	SmartPtr<SceneRender> render;

	void Run() {

	}
};