#pragma once
#include "RenderBase.h"

namespace GameEngine {
	class GameMain;

	class ScriptBase {
	public:
		ScriptBase() {}

		void Init(GameMain* engine, RenderBase** render);

		~ScriptBase() { Finalize(); }

		virtual void Run() = 0;
		virtual void Finalize() = 0;
	protected:
		const GameMain& GetEngine() const { return *engine; }
		const RenderBase& GetRender() const { return **render; }
	private:
		GameMain* engine = nullptr;
		RenderBase** render = nullptr;
	};
}
