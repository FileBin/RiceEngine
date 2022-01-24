#pragma once

#include "../../Scene/Component.h"
#include "IDrawable.h"
#include "../../Scene/SceneObject.h"
#include "../../Scene/Scene.h"
#include "../../Util/defines.h"
#include "../../Vectors/Vector2.h"
#include "../../Device.h"
#include "RectTransform.h"

namespace Game::UI {
	class UIBase : public Component, public IDrawable {
	public:
		void SetCanvas(Canvas* canv) { canvas = canv; }

		virtual void OnInit() {}

		virtual void OnEnable() {
			auto& scene = GetSceneObject().GetScene();
			if (&scene != nullptr) {
				auto& ren = scene.GetRender();
				ren.AddDrawable(this);
			}
			transform = GetSceneObject().GetComponents<RectTransform>()[0];
		}

		virtual void OnDisable() {
			auto& scene = GetSceneObject().GetScene();
			if (&scene != nullptr) {
				auto& ren = scene.GetRender();
				ren.RemoveDrawable(this);
			}
		}

	protected:

		Canvas* canvas;
		RectTransform* transform;
	};
}
