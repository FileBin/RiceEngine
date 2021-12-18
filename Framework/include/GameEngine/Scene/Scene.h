#pragma once
#include "SceneObject.h"
#include "../SceneRender.h"
#include <vector>
#include "../ScriptBase.h"

namespace Game {
	class MonoScript;

	class Scene {
	public:
		Scene();

		void Init(Engine* en) { engine = en; }

		virtual void Initialize() = 0;

		SceneRender& GetRender() { return *render; }

		void Close() {
			for (auto o : root->GetChildren()) {
				delete o;
			}
			render->Close();
			delete render;
		}

		std::vector<ScriptBase*> GetScripts() {
			return {
				initScript,
				preUpdateScript,
				updateScript,
			};
		}

		void AddScript(MonoScript* script);

		SceneObject& GetObjectByName(String name);

		SceneObject* Instaniate();
		Engine& GetEngine();

	private:
		SceneObject* root;
		SceneRender* render;

		Engine* engine;

		class InitScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			bool init = false;
			void Run();
		} *initScript;

		class PreUpdateScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			void Run();
		} *preUpdateScript;

		class UpdateScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			void Run();
		} *updateScript;
	};
}