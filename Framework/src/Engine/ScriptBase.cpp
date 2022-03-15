#include "pch.h"
#include <GameEngine/ScriptBase.h>

namespace Game {
	void ScriptBase::PreInit(Engine** _en) {
		engine = _en;
	}
	ScriptBase::~ScriptBase() { Finalize(); };
}