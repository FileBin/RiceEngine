﻿#pragma once

namespace Game {
	class SceneObject;

	class Component {
	public:
		void Init(SceneObject* parent);

		virtual void OnInit() {};

		void Enable() { OnEnable(); enabled = true; };
		virtual void OnEnable() {};
		virtual void Start() {};
		virtual void PreUpdate() {};
		virtual void Update() {};
		void Disable() { OnDisable(); enabled = false; };
		virtual void OnDisable() {};

		bool isEnabled() { return enabled; }

	protected:
		bool enabled = false;
		SceneObject& GetSceneObject();
	private:
		SceneObject* obj = nullptr;
	};
}
