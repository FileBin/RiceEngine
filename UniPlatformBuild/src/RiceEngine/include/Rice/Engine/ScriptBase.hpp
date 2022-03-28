#pragma once
#include "Engine.h"
#include "RenderBase.h"

namespace Game {

	class Engine;

	class ScriptBase {
	public:
		ScriptBase() {}
		void PreInit(Engine** engine);

		virtual ~ScriptBase() = 0;

		virtual void Init() {}
		virtual void Run() = 0;
		virtual void Finalize() {}
	protected:
		Engine& GetEngine() { return **engine; }
	private:
		Engine** engine = nullptr;
	};
}
