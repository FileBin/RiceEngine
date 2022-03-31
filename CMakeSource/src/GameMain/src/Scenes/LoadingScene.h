#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\UI\Text.h>

using namespace Game;

class LoadingScene : public Scene {
	void InitResourceManager() {
		_initResourceManager(L"LoadingSceneResources.json");
	}
	void Init() {
		auto& ren = GetRender();
		auto& en = GetEngine();

		auto& resManager = GetResourceManager();

		SmartPtr<Camera> cam = new Camera();

		cam->SetFov(90);
		cam->SetClippingPlanes(.01, 1000.);
		cam->rotation = Quaternion::identity;
		cam->position = { 1, 1, 1 };

		ren.AddCamera(cam);

		UI::RectTransform* rectTransform;
		SceneObject* obj;
		UI::Text* text = new UI::Text();
		text->SetText(L"Loading...");
		text->SetColor(D2D1::ColorF::White);

		rectTransform = new UI::RectTransform();
		rectTransform->anchor = UI::RectTransform::Anchor::MiddleCenter;
		rectTransform->SetScale({ 200,200,1 });

		obj = Instaniate();
		obj->AddComponent(rectTransform);
		obj->AddComponent(text);
	}
};
