﻿#pragma once

namespace Game {
	class SceneObject;

	class Component {
	public:
		void Init(SceneObject* parent);

		virtual ~Component() = 0;

		virtual void OnInit() {};

		virtual void Enable() {};
		virtual void Start() {};
		virtual void PreUpdate() {};
		virtual void Update() {};
		virtual void Disable() {};

		bool isEnabled() { return enabled; }

	protected:
		bool enabled = false;
		SceneObject& GetSceneObject();
	private:
		SceneObject* obj = nullptr;
	};
}
