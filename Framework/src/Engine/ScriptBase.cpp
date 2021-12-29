#include "pch.h"
#include <GameEngine/ScriptBase.h>

namespace Game {
	void ScriptBase::Init(Engine** _en, RenderBase** _ren) {
		engine = _en;
		render = _ren;
	}
	ScriptBase::~ScriptBase() { Finalize(); };
}