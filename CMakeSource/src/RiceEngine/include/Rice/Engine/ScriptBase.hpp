#include "../stdafx.hpp"

NSP_ENGINE_BEGIN

class ScriptBase;
typedef SmartPtr<ScriptBase> pScriptBase;

NSP_ENGINE_END

#pragma once

#include "Engine.hpp"
#include "RenderBase.hpp"

NSP_ENGINE_BEGIN

class ScriptBase {
public:
	ScriptBase() {
	}
	void PreInit(Engine*& engine);

	virtual ~ScriptBase() = 0;

	virtual void Init() {
	}
	virtual void Run() = 0;
	virtual void Finalize() {
	}
protected:
	Engine& GetEngine() {
		return **engine;
	}
private:
	Engine** engine = nullptr;
};\

NSP_ENGINE_END
