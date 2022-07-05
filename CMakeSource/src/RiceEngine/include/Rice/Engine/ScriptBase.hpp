#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class ScriptBase;
typedef RefPtr<ScriptBase> pScriptBase;

NSP_ENGINE_END

#pragma once

#include "Engine.hpp"
#include "RenderBase.hpp"

NSP_ENGINE_BEGIN

class ScriptBase : public ICleanable {
public:
    ScriptBase();

	virtual ~ScriptBase() = 0;

	virtual void init() {}
	virtual void run() = 0;
protected:
	pEngine getEngine() {
		return engine;
	}
private:
    pEngine& engine;
};\

NSP_ENGINE_END
