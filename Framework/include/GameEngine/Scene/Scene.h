#pragma once
#include "SceneObject.h"
#include "../SceneRender.h"
#include <vector>
#include "../ScriptBase.h"
#include <GameEngine/SoundManager.h>

namespace Game {
	class MonoScript;

	class Scene {
	public:
		Scene();

		virtual ~Scene() = 0;

		void Init(Engine* en) { engine = en; }

		virtual void Initialize() { soundManager = new SoundManager(&render->GetCamera()); }

		SceneRender& GetRender() { return *render; }

		void Close() {
			root->Disable();
			delete root;
			render->Close();
			delete render;
		}

		std::vector<ScriptBase*> GetScripts() {
			return {
				initScript,
				preUpdateScript,
				updateScript,
				closeScript
			};
		}

		void AddScript(MonoScript* script);

		SceneObject& GetObjectByName(String name);

		SceneObject* Instaniate();
		Engine& GetEngine();
		SoundManager& GetSoundManager();

		SoundManager* soundManager;
	private:
		SceneObject* root;
		SceneRender* render;

		Engine* engine;
		
		class InitScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			bool init = false;
			void Run();
			virtual ~InitScript() {};
		} *initScript;

		class PreUpdateScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			void Run();
			virtual ~PreUpdateScript() {};
		} *preUpdateScript;

		class UpdateScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			void Run();
			virtual ~UpdateScript() {};
		} *updateScript;

		class CloseScript : public ScriptBase {
		public:
			Scene* scene = nullptr;
			bool init = false;
			void Run();
		} *closeScript;
	};
}