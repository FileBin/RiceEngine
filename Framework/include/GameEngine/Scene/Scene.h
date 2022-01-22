#pragma once
#include "SceneObject.h"
#include "../SceneRender.h"
#include <vector>
#include "../ScriptBase.h"
#include "../SoundManager.h"
#include "../Physics/PhysicsEngine.h"

namespace Game {
	class MonoScript;

	class Scene {
	public:
		Scene();

		virtual ~Scene() = 0;

		void PreInit(Engine* en) {
			engine = en;
			render->Init();
		}

		virtual void Init() = 0;

		void PostInit() {
			soundManager = new SoundManager(render->GetActiveCamera());
			physicsEngine = new Physics::PhysicsEngine();
			physicsEngine->PreInit();
			root->Enable();
			init = true;
			Resize();
		}

		void Close() {
			root->Disable();
			delete root;
			render->Close();
			delete render;
		}

		void Render() {
			root->PreUpdate();
			root->Update();
			render->BeginFrame();
			render->Draw();
		}

		void Resize() {
			if (init)
				render->Resize();
		}

		void AddScript(MonoScript* script);
		void RemoveScript(MonoScript* script);

		SceneObject& GetObjectByName(String name);

		SceneObject* Instaniate();
		Engine& GetEngine() const;
		SoundManager& GetSoundManager() const;
		SceneRender& GetRender() const { return *render; }
		const SmartPtr<Physics::PhysicsEngine> GetPhysEngine() const;

		bool isLoaded() { return init; }

	private:
		bool init = false;

		SceneObject* root;
		SceneRender* render;
		SmartPtr<Physics::PhysicsEngine> physicsEngine;
		Engine* engine;
		SoundManager* soundManager;
	};
}