#pragma once
#include "SceneObject.h"
#include "../SceneRender.h"
#include <vector>
#include "../ScriptBase.h"
#include "../SoundManager.h"
#include "../Physics/PhysicsEngine.h"
#include "../RenderScript.h"

namespace Game {
	class MonoScript;

	class Scene {
	public:
		Scene();

		virtual ~Scene() = 0;

		void Init(Engine* en) { engine = en; }

		virtual void Initialize() = 0;

		void PostInit() {
			soundManager = new SoundManager(render->GetActiveCamera());
			physicsEngine = new Physics::PhysicsEngine();
			physicsEngine->Init();
		}

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
		void AddScript(RenderScript* script);

		SceneObject& GetObjectByName(String name);

		SceneObject* Instaniate();
		Engine& GetEngine() const;
		SoundManager& GetSoundManager() const;
		SceneRender& GetRender() const { return *render; }
		const SmartPtr<Physics::PhysicsEngine> GetPhysEngine() const;

		SoundManager* soundManager;
	private:
		SceneObject* root;
		SceneRender* render;
		SmartPtr<Physics::PhysicsEngine> physicsEngine;
		Engine* engine;

		std::vector<RenderScript*> renderScripts{};
		
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