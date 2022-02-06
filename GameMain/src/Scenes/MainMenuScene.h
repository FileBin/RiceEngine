#pragma once

#pragma once

#include <GameEngine\Scene\Scene.h>
#include <GameEngine\Components\ModelRender.h>
#include <GameEngine\Model.h>
#include <GameEngine\Components\UI\Text.h>
#include <GameEngine\Components\UI\Slider.h>
#include <GameEngine\Components\Rigidbody.h>

#include "../Scripts/CameraMoverScript.h"
#include "../Scripts/ChunkGenerator.h"
#include "../Scripts/UI/DebugText.h"
#include "../Scripts/PlayerHandler.h"
#include "../Scripts/SamplePostProcess.h"

#include "SampleScene.h"


using namespace Game;

class MainMenuScene : public Scene {
	void Init() {
		auto& ren = GetRender();
		auto& en = GetEngine();

		SmartPtr<Camera> cam = new Camera();

		cam->SetFov(90);
		cam->SetClippingPlanes(.01, 1000.);
		cam->rotation = Quaternion::identity;
		cam->position = { 1, 1, 1 };

		ren.AddCamera(cam);

		//start button
		UI::Button* button = new UI::Button();
		button->SetImg(&ren.CreateTexture(L"img/sky_finger.png"));
		button->setOnClickListener([&en](UI::Button* sender) {
				en.LoadScene(new SampleScene());
			});

		auto rectTransform = new UI::RectTransform();
		rectTransform->anchor = UI::RectTransform::Anchor::TopCenter;
		rectTransform->SetPosition({0,0});
		rectTransform->SetScale({ 200,33 });

		auto StartButton = Instaniate();
		StartButton->SetName(L"StartButton");
		StartButton->AddComponent(rectTransform);
		StartButton->AddComponent(button);
	}
};
