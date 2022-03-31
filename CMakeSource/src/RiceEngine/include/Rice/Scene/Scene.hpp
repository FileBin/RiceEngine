#pragma once
#include "SceneObject.h"
#include "SceneRender.h"
#include <vector>
#include "../ScriptBase.h"
#include "../SoundManager.h"
#include "../Physics/PhysicsEngine.h"
#include "ResourceManager.h"

namespace Game {
	class MonoScript;

	class Scene {
	private:
		friend class SceneRender;
	public:
		Scene();
		virtual ~Scene() {}

		void PreInit(Engine* en, Device* device) {
			render->SetDevice(device);
			InitResourceManager(); 
			engine = en;
			render->Init();
		}

		virtual void InitResourceManager() = 0;

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
			_DELETE(soundManager);
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

		SceneObject* Instaniate(SceneObject* orig);
		SceneObject* Instaniate();
		Engine& GetEngine() const;
		ResourceManager& GetResourceManager() { return resManager; }
		SoundManager& GetSoundManager() const;
		SceneRender& GetRender() const { return *render; }
		const SmartPtr<Physics::PhysicsEngine> GetPhysEngine() const;

		bool isLoaded() { return init; }

	protected:
		void _initResourceManager(String path) {
			resManager.LoadJson(path);
		}

	private:
		bool init = false;

		ResourceManager resManager;
		SceneObject* root;
		SceneRender* render;
		SmartPtr<Physics::PhysicsEngine> physicsEngine;
		Engine* engine;
		SoundManager* soundManager;
	};
}