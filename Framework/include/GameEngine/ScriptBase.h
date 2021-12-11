#pragma once
#include "Engine.h"
#include "RenderBase.h"

namespace GameEngine {

	class Engine;

	class ScriptBase {
	public:
		ScriptBase() {}
		void Init(Engine** engine, RenderBase** render);

		~ScriptBase() { Finalize(); }

		virtual void Initialize() {}
		virtual void Run() = 0;
		virtual void Finalize() {}
	protected:
		Engine& GetEngine() { return **engine; }
		RenderBase& GetRender() { return **render; }
	private:
		Engine** engine = nullptr;
		RenderBase** render = nullptr;
	};
}
