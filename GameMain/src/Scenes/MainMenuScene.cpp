#include "pch.h"
#include "MainMenuScene.h"

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

void MainMenuScene::Init() {
	auto& ren = GetRender();
	auto& en = GetEngine();

	InitResourceManager(L"resources/MainMenuSceneResources.json");

	auto& resManager = GetResourceManager();

	SmartPtr<Camera> cam = new Camera();

	cam->SetFov(90);
	cam->SetClippingPlanes(.01, 1000.);
	cam->rotation = Quaternion::identity;
	cam->position = { 1, 1, 1 };

	ren.AddCamera(cam);

	//start button
	UI::Button* button = new UI::Button();
	button->SetImg(&ren.CreateTexture(resManager.GetString("StartButtonImg")));
	button->setOnClickListener([&en](UI::Button* sender) {
		en.LoadScene(new SampleScene());
		});

	auto rectTransform = new UI::RectTransform();
	rectTransform->anchor = UI::RectTransform::Anchor::MiddleCenter;
	rectTransform->SetPosition({ 0,-100 });
	rectTransform->SetScale({ 200,33 });

	auto StartButton = Instaniate();
	StartButton->SetName(L"StartButton");
	StartButton->AddComponent(rectTransform);
	StartButton->AddComponent(button);

	//slider
	UI::Slider* slider = new UI::Slider();
	slider->SetImg(&ren.CreateTexture(resManager.GetString("CrossTexture")));
	slider->SetBgImg(&ren.CreateTexture(L"img/sky_finger.png"));

	rectTransform = new UI::RectTransform();
	rectTransform->anchor = UI::RectTransform::Anchor::BottomCenter;
	rectTransform->SetPosition({ 0,-20 });
	rectTransform->SetScale({ 200,55 });

	auto Slider = Instaniate();
	Slider->SetName(L"Slider");
	Slider->AddComponent(rectTransform);
	Slider->AddComponent(slider);
}